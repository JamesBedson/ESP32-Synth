#include "Utils/AudioParameterTree.h"
#include "Web/WebMessage.h"
#include "Web/WebMessageHandler.h"
#include "IParameterListener.h"
#include "Utils/LockFreeQueue.h"

// Middle man class that knows how to
// - Take UI messages and forward them to ParameterTree
// - Take parameter change notifications and send them to WebMessageHandler

class MessageBridge : public IParameterListener 
{

public:
    MessageBridge(
        AudioParameterTree& parameterTree,
        WebMessageHandler& webHandler
    ) : tree(parameterTree), handler(webHandler) 
    {
        tree.addParameterListener(this);
    }

    // UI --> DSP
    void handle(const WebMessage& msg) {
        switch (msg.type) {

            case WebCommandType::GetParameter: {
                float value = tree.getAsFloat(msg.paramID);
                handler.sendParameterUpdate(msg.paramID, value);
                break;
            }

            case WebCommandType::SetParameter: {
                tree.setParameter(msg.paramID, msg.value);
                break;
            }
        }
    }

    // DSP --> UI
    void onParameterChanged(int index, float value) override {
        queue.push(
            { 
                index,
                value
            }
        );
    }

    void processPendingMessages() {
        PendingParam msg;

        while (queue.pop(msg)) {
            const std::string& id = tree.getIDFromIndex(msg.index);
            handler.sendParameterUpdate(id, msg.value);
        }
    }


private:
    struct PendingParam {
        int index;
        float value;
    };

    static constexpr size_t QUEUE_SIZE = 256;
    LockFreeQueue<PendingParam, QUEUE_SIZE> queue;

    AudioParameterTree& tree;
    WebMessageHandler& handler;    
};