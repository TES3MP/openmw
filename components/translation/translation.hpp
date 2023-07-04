#ifndef COMPONENTS_TRANSLATION_DATA_H
#define COMPONENTS_TRANSLATION_DATA_H

#include <components/files/collections.hpp>
#include <components/to_utf8/to_utf8.hpp>

namespace Translation
{
    class Storage
    {
    public:
        Storage();

        void loadTranslationData(const Files::Collections& dataFileCollections, std::string_view esmFileName);

        std::string_view translateCellName(std::string_view cellName) const;
        std::string_view topicID(std::string_view phrase) const;

        // Standard form usually means nominative case
        std::string_view topicStandardForm(std::string_view phrase) const;

        void setEncoder(ToUTF8::Utf8Encoder* encoder);

        bool hasTranslation() const;

        /*
            Start of tes3mp addition

            Get the localized version of an English topic ID
        */
        std::string getLocalizedTopicId(const std::string& englishTopicId) const;
        /*
            End of tes3mp addition
        */

    private:
        typedef std::map<std::string, std::string, std::less<>> ContainerType;

        void loadData(ContainerType& container, const std::string& fileNameNoExtension, const std::string& extension,
            const Files::Collections& dataFileCollections);

        void loadDataFromStream(ContainerType& container, std::istream& stream);

        ToUTF8::Utf8Encoder* mEncoder;
        ContainerType mCellNamesTranslations, mTopicIDs, mPhraseForms;
    };
}

#endif
