#include "openxrviewer.hpp"
#include "openxrmanagerimpl.hpp"
#include "Windows.h"
#include "../mwrender/vismask.hpp"
#include "../mwmechanics/actorutil.hpp"
#include "../mwbase/world.hpp"
#include "../mwbase/environment.hpp"
#include "../mwworld/class.hpp"
#include "../mwworld/player.hpp"
#include "../mwworld/esmstore.hpp"
#include <components/esm/loadrace.hpp>
#include <osg/MatrixTransform>

namespace MWVR
{

    OpenXRViewer::OpenXRViewer(
        osg::ref_ptr<OpenXRManager> XR,
        osg::ref_ptr<osgViewer::Viewer> viewer,
        float metersPerUnit)
        : osg::Group()
        , mXR(XR)
        , mRealizeOperation(new RealizeOperation(XR, this))
        , mViewer(viewer)
        , mMetersPerUnit(metersPerUnit)
        , mConfigured(false)
        , mCompositionLayerProjectionViews(2, {XR_TYPE_COMPOSITION_LAYER_PROJECTION_VIEW})
        , mXRSession(nullptr)
        , mPreDraw(new PredrawCallback(this))
        , mPostDraw(new PostdrawCallback(this))
    {
        mViewer->setRealizeOperation(mRealizeOperation);
        mCompositionLayerProjectionViews[0].pose.orientation.w = 1;
        mCompositionLayerProjectionViews[1].pose.orientation.w = 1;
        this->setName("OpenXRRoot");

        this->setUserData(new TrackedNodeUpdateCallback(this));

        mLeftHandTransform->setName("tracker l hand");
        mLeftHandTransform->setUpdateCallback(new TrackedNodeUpdateCallback(this));
        this->addChild(mLeftHandTransform);

        mRightHandTransform->setName("tracker r hand");
        mRightHandTransform->setUpdateCallback(new TrackedNodeUpdateCallback(this));
        this->addChild(mRightHandTransform);
    }

    OpenXRViewer::~OpenXRViewer(void)
    {
    }

    void 
        OpenXRViewer::traverse(
            osg::NodeVisitor& visitor)
    {
        if (mRealizeOperation->realized())
            osg::Group::traverse(visitor);
    }

    const XrCompositionLayerBaseHeader*
        OpenXRViewer::layer()
    {
        // Does not check for visible, since this should always render
        return reinterpret_cast<XrCompositionLayerBaseHeader*>(mLayer.get());
    }

    void OpenXRViewer::traversals()
    {
        mXR->handleEvents();
        mViewer->updateTraversal();
        mViewer->renderingTraversals();
    }

    void OpenXRViewer::realize(osg::GraphicsContext* context)
    {
        std::unique_lock<std::mutex> lock(mMutex);

        if (mConfigured)
            return;

        if (!context->isCurrent())
        if (!context->makeCurrent())
        {
            throw std::logic_error("OpenXRViewer::configure() failed to make graphics context current.");
            return;
        }

        

        auto mainCamera = mCameras["MainCamera"] = mViewer->getCamera();
        mainCamera->setName("Main");
        mainCamera->setInitialDrawCallback(new OpenXRView::InitialDrawCallback());

        // Use the main camera to render any GUI to the OpenXR GUI quad's swapchain.
        // (When swapping the window buffer we'll blit the mirror texture to it instead.)
        mainCamera->setCullMask(MWRender::Mask_GUI);

        osg::Vec4 clearColor = mainCamera->getClearColor();

        if (!mXR->realized())
            mXR->realize(context);

        OpenXRSwapchain::Config leftConfig;
        leftConfig.width = mXR->impl().mConfigViews[(int)Chirality::LEFT_HAND].recommendedImageRectWidth;
        leftConfig.height = mXR->impl().mConfigViews[(int)Chirality::LEFT_HAND].recommendedImageRectHeight;
        leftConfig.samples = mXR->impl().mConfigViews[(int)Chirality::LEFT_HAND].recommendedSwapchainSampleCount;
        OpenXRSwapchain::Config rightConfig;
        rightConfig.width = mXR->impl().mConfigViews[(int)Chirality::RIGHT_HAND].recommendedImageRectWidth;
        rightConfig.height = mXR->impl().mConfigViews[(int)Chirality::RIGHT_HAND].recommendedImageRectHeight;
        rightConfig.samples = mXR->impl().mConfigViews[(int)Chirality::RIGHT_HAND].recommendedSwapchainSampleCount;

        auto leftView = new OpenXRWorldView(mXR, "LeftEye", context->getState(), leftConfig, mMetersPerUnit);
        auto rightView = new OpenXRWorldView(mXR, "RightEye", context->getState(), rightConfig, mMetersPerUnit);

        mViews["LeftEye"] = leftView;
        mViews["RightEye"] = rightView;

        auto leftCamera = mCameras["LeftEye"] = leftView->createCamera(0, clearColor, context);
        auto rightCamera = mCameras["RightEye"] = rightView->createCamera(1, clearColor, context);

        leftCamera->setPreDrawCallback(mPreDraw);
        rightCamera->setPreDrawCallback(mPreDraw);

        leftCamera->setPostDrawCallback(mPostDraw);
        rightCamera->setPostDrawCallback(mPostDraw);

        // Stereo cameras should only draw the scene (AR layers should later add minimap, health, etc.)
        leftCamera->setCullMask(~MWRender::Mask_GUI);
        rightCamera->setCullMask(~MWRender::Mask_GUI);

        leftCamera->setName("LeftEye");
        rightCamera->setName("RightEye");

        mViewer->addSlave(leftCamera, leftView->projectionMatrix(), leftView->viewMatrix(), true);
        mViewer->addSlave(rightCamera, rightView->projectionMatrix(), rightView->viewMatrix(), true);


        mViewer->setLightingMode(osg::View::SKY_LIGHT);
        mViewer->setReleaseContextAtEndOfFrameHint(false);

        mCompositionLayerProjectionViews[0].subImage = leftView->swapchain().subImage();
        mCompositionLayerProjectionViews[1].subImage = rightView->swapchain().subImage();


        OpenXRSwapchain::Config config;
        config.width = mainCamera->getViewport()->width();
        config.height = mainCamera->getViewport()->height();
        config.samples = 1;

        // Mirror texture doesn't have to be an OpenXR swapchain.
        // It's just convenient.
        mMirrorTextureSwapchain.reset(new OpenXRSwapchain(mXR, context->getState(), config));

        auto menuView = new OpenXRMenu(mXR, config, context->getState(), "MainMenu", osg::Vec2(1.f, 1.f));
        mViews["MenuView"] = menuView;

        auto menuCamera = mCameras["MenuView"] = menuView->createCamera(2, clearColor, context);
        menuCamera->setCullMask(MWRender::Mask_GUI);
        menuCamera->setName("MenuView");
        menuCamera->setPreDrawCallback(mPreDraw);
        menuCamera->setPostDrawCallback(mPostDraw);

        mViewer->addSlave(menuCamera, true);

        mXRSession.reset(new OpenXRSession(mXR));
        mViewer->getSlave(0)._updateSlaveCallback = new OpenXRWorldView::UpdateSlaveCallback(mXR, mXRSession.get(), leftView, context);
        mViewer->getSlave(1)._updateSlaveCallback = new OpenXRWorldView::UpdateSlaveCallback(mXR, mXRSession.get(), rightView, context);

        mainCamera->getGraphicsContext()->setSwapCallback(new OpenXRViewer::SwapBuffersCallback(this));
        mainCamera->setGraphicsContext(nullptr);
        mXRSession->setLayer(OpenXRLayerStack::WORLD_VIEW_LAYER, this);
        mXRSession->setLayer(OpenXRLayerStack::MENU_VIEW_LAYER, dynamic_cast<OpenXRLayer*>(mViews["MenuView"].get()));
        mConfigured = true;

    }

    void OpenXRViewer::blitEyesToMirrorTexture(osg::GraphicsContext* gc, bool includeMenu)
    {
        mMirrorTextureSwapchain->beginFrame(gc);

        int mirror_width = 0;
        if(includeMenu)
            mirror_width = mMirrorTextureSwapchain->width() / 3;
        else
            mirror_width = mMirrorTextureSwapchain->width() / 2;


        mViews["LeftEye"]->swapchain().renderBuffer()->blit(gc, 0, 0, mirror_width, mMirrorTextureSwapchain->height());
        mViews["RightEye"]->swapchain().renderBuffer()->blit(gc, mirror_width, 0, 2 * mirror_width, mMirrorTextureSwapchain->height());

        if(includeMenu)
            mViews["MenuView"]->swapchain().renderBuffer()->blit(gc, 2 * mirror_width, 0, 3 * mirror_width, mMirrorTextureSwapchain->height());

        mMirrorTextureSwapchain->endFrame(gc);



        auto* state = gc->getState();
        auto* gl = osg::GLExtensions::Get(state->getContextID(), false);
        gl->glBindFramebuffer(GL_FRAMEBUFFER_EXT, 0);
        mMirrorTextureSwapchain->renderBuffer()->blit(gc, 0, 0, mMirrorTextureSwapchain->width(), mMirrorTextureSwapchain->height());
    }

    void
        OpenXRViewer::SwapBuffersCallback::swapBuffersImplementation(
            osg::GraphicsContext* gc)
    {
        mViewer->mXRSession->swapBuffers(gc);
    }

    void OpenXRViewer::swapBuffers(osg::GraphicsContext* gc)
    {

        if (!mConfigured)
            return;

        ////// NEW SYSTEM
        Timer timer("OpenXRViewer::SwapBuffers");
        mViews["LeftEye"]->swapBuffers(gc);
        mViews["RightEye"]->swapBuffers(gc);
        timer.checkpoint("Views");

        auto eyePoses = mXRSession->predictedPoses().eye; 
        auto leftEyePose = toXR(mViews["LeftEye"]->predictedPose());
        auto rightEyePose = toXR(mViews["RightEye"]->predictedPose());
        mCompositionLayerProjectionViews[0].pose = leftEyePose;
        mCompositionLayerProjectionViews[1].pose = rightEyePose;
        timer.checkpoint("Poses");

        // TODO: Keep track of these in the session too.
        auto stageViews = mXR->impl().getPredictedViews(mXR->impl().frameState().predictedDisplayTime, TrackedSpace::STAGE);
        mCompositionLayerProjectionViews[0].fov = stageViews[0].fov;
        mCompositionLayerProjectionViews[1].fov = stageViews[1].fov;
        timer.checkpoint("Fovs");


        if (!mLayer)
        {
            mLayer.reset(new XrCompositionLayerProjection);
            mLayer->type = XR_TYPE_COMPOSITION_LAYER_PROJECTION;
            mLayer->space = mXR->impl().mReferenceSpaceStage;
            mLayer->viewCount = 2;
            mLayer->views = mCompositionLayerProjectionViews.data();
        }

        blitEyesToMirrorTexture(gc);

        gc->swapBuffersImplementation();
    }

    void
        OpenXRViewer::RealizeOperation::operator()(
            osg::GraphicsContext* gc)
    {
        OpenXRManager::RealizeOperation::operator()(gc);
        mViewer->realize(gc);
    }

    bool
        OpenXRViewer::RealizeOperation::realized()
    {
        return mViewer->realized();
    }

    void OpenXRViewer::preDrawCallback(osg::RenderInfo& info)
    {
        auto* camera = info.getCurrentCamera();
        auto name = camera->getName();
        auto& view = mViews[name];

        if (name == "LeftEye")
        {
            if (mXR->sessionRunning())
            {
                mXR->beginFrame();
                auto& poses = mXRSession->predictedPoses();
                auto menuPose = poses.head[(int)TrackedSpace::STAGE];
                mViews["MenuView"]->setPredictedPose(menuPose);
            }
        }

        view->prerenderCallback(info);
    }

    void OpenXRViewer::postDrawCallback(osg::RenderInfo& info)
    {
        auto* camera = info.getCurrentCamera();
        auto name = camera->getName();
        auto& view = mViews[name];

        view->postrenderCallback(info);

        // OSG will sometimes overwrite the predraw callback.
        if (camera->getPreDrawCallback() != mPreDraw)
        {
            camera->setPreDrawCallback(mPreDraw);
            Log(Debug::Warning) << ("osg overwrote predraw");
        }
    }

    void OpenXRViewer::updateTransformNode(osg::Object* object, osg::Object* data)
    {
        auto* hand_transform = dynamic_cast<SceneUtil::PositionAttitudeTransform*>(object);
        if (!hand_transform)
        {
            Log(Debug::Error) << "Update node was not PositionAttitudeTransform";
            return;
        }

        auto& poses = mXRSession->predictedPoses();
        auto handPosesView = poses.hands[(int)TrackedSpace::VIEW];
        auto handPosesStage = poses.hands[(int)TrackedSpace::STAGE];
        int chirality = (int)Chirality::LEFT_HAND;
        if (hand_transform->getName() == "tracker r hand")
            chirality = (int)Chirality::RIGHT_HAND;

        MWVR::Pose hand = handPosesStage[chirality];
        mXR->playerScale(hand);
        auto orientation = hand.orientation;
        auto position = hand.position;
        position = position * mMetersPerUnit;

        // Move OpenXR's poses into OpenMW's view by applying the inverse of the rotation of the view matrix.
        // This works because OpenXR's conventions match opengl's clip space, thus the inverse of the view matrix converts an OpenXR pose to OpenMW's view space (including world rotation).
        // For the hands we don't want the full camera view matrix, but the relative matrix from the player root. So i create a lookat matrix based on osg's conventions.
        // TODO: The full camera view matrix could work if i change how animations are overriden.
        osg::Matrix lookAt;
        lookAt.makeLookAt(osg::Vec3(0, 0, 0), osg::Vec3(0, 1, 0), osg::Vec3(0, 0, 1));
        lookAt = osg::Matrix::inverse(lookAt);

        orientation = orientation * lookAt.getRotate();
        //position = invViewMatrix.preMult(position);
        position = lookAt.getRotate() * position;
        
        // Morrowind's meshes do not point forward by default.
        // Static since they do not need to be recomputed.
        static float VRbias = osg::DegreesToRadians(-90.f);
        static osg::Quat yaw(VRbias, osg::Vec3f(0, 1, 0));
        static osg::Quat pitch(2.f * VRbias, osg::Vec3f(0, 0, 1));
        static osg::Quat roll (-VRbias, osg::Vec3f(1, 0, 0));

        orientation = pitch * yaw * orientation;


        if (hand_transform->getName() == "tracker r hand")
            orientation = roll * orientation;
        else
            orientation = roll.inverse() * orientation;
        
        // Hand are by default not well-centered
        // Note, these numbers are just a rough guess, but seem to work out well.
        osg::Vec3 offcenter = osg::Vec3(-0.175, 0., .033);
        if (hand_transform->getName() == "tracker r hand")
            offcenter.z() *= -1.;
        osg::Vec3 recenter = orientation * offcenter;
        position = position + recenter * mMetersPerUnit;

        hand_transform->setAttitude(orientation);
        hand_transform->setPosition(position);
    }

    bool
        OpenXRViewer::TrackedNodeUpdateCallback::run(
            osg::Object* object, 
            osg::Object* data)
    {
        mViewer->updateTransformNode(object, data);
        return traverse(object, data);
    }
}
