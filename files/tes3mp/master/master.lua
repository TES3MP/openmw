function script_path()
   local str = debug.getinfo(2, "S").source:sub(2)
   return str:match("(.*/)")
end

-------------------------------------[response strings]-------------------------------------

response_ok = "HTTP/1.1 200 OK\r\nContent-Length: 2\r\n\r\nOK"
response_bad = "HTTP/1.1 400 Bad Request\r\nContent-Length: 11\r\n\r\nbad request"
response_forbidden = "HTTP/1.1 403 Forbidden\r\nContent-Length: 9\r\n\r\nforbidden"
response_error = "HTTP/1.1 500 Internal Server Error\r\nContent-Length: 21\r\n\r\nInternal Server Error"

function ResponseStr(str)
    if type(str) ~= "string" then return response_error end

    return "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\nContent-Length: " .. #str .. "\r\n\r\n" .. str
end

-------------------------------------[config]-------------------------------------
config = {
    maxConnections = 10000,
    port = 25560,
    restPort = 8080,

    restAdminPort = 4040,
    restAdminCert = script_path().."cert/master.crt",
    restAdminKey =  script_path().."cert/master.key",
    restAdminVerifyFile = script_path().."cert/rootCA.crt"
}

admins = require("admins")
bansFile = script_path().."/".."banned.json"
-------------------------------------[logic]-------------------------------------

loggedIn = {} -- {{"koncord", "127.0.0.1"}}
banned = {}
if type(admins) ~= "table" then
    error("\"admins.lua\" is not configured.")
end

local json = require ("dkjson");

function ban(addr, admin, reason, dontInsert)
    local entry = {
        address = addr,
        date = tonumber(os.date("%s")),
        by = admin,
        reason = reason
    }

    if dontInsert == nil then  -- if dontInsert is not provided to arguments then entry will not be inserted
        table.insert(banned, entry)
    end
    BanAddress(addr)
end

function unban(addr)
    for idx, value in pairs(banned) do
        if value.address == addr then
            table.remove(banned, idx)
            UnbanAddress(addr)
            return true
        end
    end
    return false
end

function loadBans()
    local file = assert(io.open(bansFile, 'r'), 'Error loading file: ' .. bansFile)
    local content = file:read("*all")
    file:close()

    banned = json.decode(content, 1, nil)
    for _, entry in pairs(banned) do
        ban(entry.address, entry.admin, entry.reason, true)
    end
end

function saveBans()
    local content = json.encode(banned, { indent = true, --[[keyorder = {"address", "date", "by", "reason"}]]})
    if content ~= "null" then
        local file = assert(io.open(bansFile, 'w+b'), 'Error loading file: ' .. bansFile)
        file:write(content)
        file:close()
    end
end

-- _type 1 for login, 2 for address
function LoginId(_type, value)
    for k, entry in ipairs(loggedIn) do
        if entry[_type] == value then
            return k
        end
    end
    return nil
end


function LogOut(address)
    local loginId = LoginId(2, address)
    if loginId == nil then return end

    table.remove(loggedIn, loginId)
end


-- returns false if response is not modified 
-- returns true and response if response is modified
function OnCommand(command, v, address)

    print("Command: " .. command)
    if command == "login" then

        print("Login attempt.")
        print("Login: "..v["login"])
        print("PassMD5: "..v["passMD5"])

        local loginId = LoginId(1, v["login"])

        if loginId ~= nil then
             print("Already logged in with address: " .. loggedIn[loginId][2])
             return true, response_forbidden
        end

        for _, entry in ipairs(admins) do -- find user in admins list
            if entry[1] == v["login"] and entry[2] == v["passMD5"] then
                table.insert(loggedIn, {entry[1], address})
                return false -- ok, user found and logged in
            end
        end
        return true, response_forbidden
    end

    local loginId = LoginId(2, address)

    if loginId == nil then
        print("User with address \"" .. address .. "\" is not logged in")
        return true, response_forbidden
    end

    if command == "ban" then
        print("Ban address: " .. v["address"])
        ban(v["address"], loggedIn[loginId][1], v["reason"])
    elseif command == "unban" then
        print("Unban address: " .. v["address"])
        if not unban(v["address"]) then -- if server not found
            return true, response_bad
        end
    elseif command == "banlist" then -- can be used only as last command
        local data = json.encode(banned, { indent = true--[[, keyorder = {"address", "date", "by", "reason"} ]] })
        return true, ResponseStr(data)
    elseif command == "savebans" then
        saveBans()
    end

    return false
end

-------------------------------------[callbacks]-------------------------------------

function OnAdminRequest(address, request) -- callback

    local content = json.decode(request, 1, nil)["content"];
    local response = response_ok

    for k, v in ipairs(content) do
        local stop = false
        for k2, v2 in pairs(v) do
            if k2 == "command" then
                local ret, resp = OnCommand(v2, v, address)
                if ret == true then
                    response = resp
                    stop = true
                end
                break
            end
        end
        if stop == true then
            break
        end
    end

    LogOut(address)

    return response
end

function OnServerAnnounce(address, sserver) -- callback
    print(sserver.name)
    print(sserver.gamemode)
    print(sserver.version)
    return true
end

function OnInit() -- called when master server thread was started
    loadBans()
end

function OnExit() -- callback
    saveBans()
end

