#pragma once

// Std
#include <list>
#include <mutex>
#include <string>
#include <thread>
#include <vector>
#include <cstdint>
#include <atomic>
#include <tuple>
#include <chrono>
#include <unordered_map>

// Libraries
#include <XLinkPublicDefines.h>

// Shared
#include "depthai-shared/stream/stream_info.hpp"
#include "depthai-shared/general/data_observer.hpp"
#include "depthai-shared/general/data_subject.hpp"
#include "depthai-shared/stream/stream_data.hpp"


namespace dai {


struct DeviceInfo {
    deviceDesc_t desc;
    XLinkDeviceState_t state;
};

class XLinkConnection {

    static std::atomic<bool> xlinkGlobalInitialized;
    static XLinkGlobalHandler_t xlinkGlobalHandler;
    static void initXLinkGlobal();
    static std::mutex xlinkStreamOperationMutex;

public:
    
    // static API
    static std::vector<DeviceInfo> getAllConnectedDevices();
    static std::tuple<bool, DeviceInfo> getFirstDevice(XLinkDeviceState_t state);


    XLinkConnection(const DeviceInfo& deviceDesc, std::vector<std::uint8_t> mvcmdBinary);
    XLinkConnection(const DeviceInfo& deviceDesc, std::string pathToMvcmd);
    XLinkConnection(const DeviceInfo& deviceDesc);

    ~XLinkConnection();

    void setRebootOnDestruction(bool reboot);
    bool getRebootOnDestruction();

    void openStream(const std::string& streamName, std::size_t maxWriteSize);
    void closeStream(const std::string& streamName);
    void writeToStream(const std::string& streamName, std::uint8_t* data, std::size_t size);
    void writeToStream(const std::string& streamName, std::vector<std::uint8_t> data);
    std::vector<std::uint8_t> readFromStream(const std::string& streamName);
    void readFromStream(const std::string& streamName, std::vector<std::uint8_t>& data );

    // USE ONLY WHEN COPYING DATA AT LATER STAGES
    streamPacketDesc_t* readFromStreamRaw(const std::string& streamName);
    // USE ONLY WHEN COPYING DATA AT LATER STAGES
    void readFromStreamRawRelease(const std::string& streamName);
    

    int getLinkId(){
        return deviceLinkId;
    }

private:
    void initDevice(const DeviceInfo& deviceToInit);
    bool bootAvailableDevice(const deviceDesc_t& deviceToBoot, std::string pathToMvcmd);
    bool bootAvailableDevice(const deviceDesc_t& deviceToBoot, std::vector<std::uint8_t> mvcmd);
    std::string convertErrorCodeToString(XLinkError_t error_code) const;

    std::unordered_map<std::string, streamId_t> streamIdMap;

    bool bootDevice = true;
    bool bootWithPath = true;
    std::string pathToMvcmd;
    std::vector<std::uint8_t> mvcmd;

    bool rebootOnDestruction{false};


    int deviceLinkId = -1;

    XLinkHandler_t connectionHandler;


    const std::chrono::milliseconds WAIT_FOR_BOOTUP_TIMEOUT{1000};
    const std::chrono::milliseconds WAIT_FOR_CONNECT_TIMEOUT{1000};
    const int STREAM_OPEN_RETRIES = 5;
    const std::chrono::milliseconds WAIT_FOR_STREAM_RETRY{50};


};


}