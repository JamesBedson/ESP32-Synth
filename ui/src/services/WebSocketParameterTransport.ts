import type { WebSocketService } from "./WebSocketService";
import type { AudioParameter } from "../types/AudioParameter";
import type { WebMessage } from "../types/Messages";

export class WebSocketParameterTransport {
    private unsubscribers: Array<() => void> = [];

    public constructor(private ws: WebSocketService) {}

    public bind(parameterSet: { all(): AudioParameter<any>[] }) {
        this.unbind();

        const params = parameterSet.all();

        // Param --> WebSocket
        for (const p of params) {
            const off = p.onChange(
                (value: any) => {

                    const msg: WebMessage = {
                        type: "set",
                        id: p.id,
                        value
                    };
                    this.ws.send(msg);
                }
            );
            this.unsubscribers.push(off);
        }

        // WebSocket --> Param
        for (const p of params) {
            this.ws.onMessage(p.id, (msg: WebMessage) => {
                if (msg.value === undefined) return;
                p.set(msg.value);
            });
        }
    }

    public unbind() {
        for (const off of this.unsubscribers)
            off();
        this.unsubscribers = [];
    }
}
