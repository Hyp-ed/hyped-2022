#include <stdlib.h>
#include <string.h>
#include <stdint.h>

namespace hyped::utils::io {

struct ProtocolState {
    enum Enum 
    {
        // The serial object was not set
        NO_SERIAL = 0,

         // The operation succeeded
        SUCCESS = 1,

        // There is not (enough) data to process
        NO_DATA = 2,
        
         // The object is being received but the buffer doesn't have all the data
        WAITING_FOR_DATA = 3,

        // The size of the received payload doesn't match the expected size
        INVALID_SIZE = 4,

        // The object was received but it is not the same as one sent
        INVALID_CHECKSUM = 5
    };
};

class SerialProtocol {
    public:
        SerialProtocol(int, uint8_t*, uint8_t);

        // Sends the current payload
        
        // Returns a ProtocolState enum value
        uint8_t send();

        // Tries to receive the payload from the
        // current available data
        // Will replace the payload if the receive succeeds

        // Returns a ProtocolState enum value
        uint8_t receive();
        
        void setSerial(int);
        bool serialAvailable();
        void sendData(uint8_t);
        uint8_t readData();

    private:
        int serial;
        uint8_t* payload;
        uint8_t payloadSize;

        uint8_t* inputBuffer;
        uint8_t bytesRead;

        uint8_t actualChecksum;
    };
};