#include "../ParameterUtils/AudioParameterTree.h"
#include "../Web/WebMessage.h"
#include "../Web/WebMessageHandler.h"
#include "../ParameterUtils/IParameterListener.h"
#include "../Utils/LockFreeQueue.h"

// Middle man class that knows how to
// - Take UI messages and forward them to ParameterTree
// - Take parameter change notifications and send them to WebMessageHandler

class MessageBridge : public IParameterListener
{
public:
    MessageBridge(AudioParameterTree &parameterTree)
        : tree(parameterTree)
    {
        tree.addParameterListener(this);
    }

    void connectToWebHandler(WebMessageHandler &handler)
    {
        // UI → DSP
        handler.sendUpdateToBackend =
            [this](const WebMessage &msg)
        {
            this->handleUIMessage(msg);
        };

        // DSP → UI
        sendToUI =
            [&](const WebMessage &msg)
        {
            handler.sendMessageToUI(msg);
        };
    }

    // UI requests → DSP
    void handleUIMessage(const WebMessage &msg)
    {
        switch (msg.type)
        {
            // UI asks DSP for parameter update
            case WebMessageType::GetParameter:
            {
                float value = tree.getAsFloat(msg.paramID);

                if (sendToUI)
                {
                    WebMessage reply;
                    reply.type = WebMessageType::Param;
                    reply.paramID = msg.paramID;
                    reply.value = value;

                    sendToUI(reply);
                }
                break;
            }
            
            // UI requests to send a parameter change
            case WebMessageType::SetParameter:
            {
                suppressUIFeedback = true;
                tree.setParameter(msg.paramID, msg.value);
                suppressUIFeedback = false;
                break;
            }
        }
    }

    // Bridge sends → UI
    void processPendingMessages()
    {
        if (!sendToUI)
            return;

        PendingParam p;
        while (queue.pop(p))
        {
            WebMessage wm;
            wm.type = WebMessageType::Param;               // DSP → UI update
            wm.paramID = tree.getIDFromIndex(p.index);     // parameter ID
            wm.value = p.value;                            // new value
            
            sendToUI(wm);  // WebMessage → WebMessageHandler → JSON → UI
        }
    }

    // DSP → Bridge
    void onParameterChanged(int index, float value) override
    {
        if (suppressUIFeedback)
            return;
            
        queue.push({index, value});
    }

private:
    struct PendingParam
    {
        int index;
        float value;
    };

    static constexpr size_t QUEUE_SIZE = 256;
    LockFreeQueue<PendingParam, QUEUE_SIZE> queue;

    AudioParameterTree &tree;
    std::function<void(const WebMessage &)> sendToUI;
};
