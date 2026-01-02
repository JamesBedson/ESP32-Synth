import type { WebMessage } from "../types/Messages.js";

export class WebSocketService {

    private ws: WebSocket | null = null;
    private reconnectTimeout: number | null = null;
    private messageHandlers: Map<string, (msg: WebMessage) => void> = new Map();

    constructor(private url: string) {
        this.connect();
    }

    private connect(): void {
        console.log("Connecting to WebSocket:", this.url);
        this.ws = new WebSocket(this.url);

        this.ws.onopen = () => {
            console.log("WebSocket connected");
        };

        this.ws.onmessage = (event: MessageEvent) => {
            try {
                const msg: WebMessage = JSON.parse(event.data);
                console.log("Message Received:", msg);

                const handler = this.messageHandlers.get(msg.id);
                if (handler) {
                    handler(msg);
                }
            }

            catch (error) {
                console.error("Failed to parse message:", error);
            }
        };

        this.ws.onerror = (error) => {
            console.error("Websocket error:", error);
        }

        this.ws.onclose = () => {
            console.log("Websocket closed, reconnecting...");
            this.reconnectTimeout = window.setTimeout(
                () => this.connect(), 2000
            );
        }
    }

    public send(msg: WebMessage): void {
        
        if (this.ws?.readyState === WebSocket.OPEN) {
            this.ws.send(JSON.stringify(msg));
            console.log("Sent:", msg);
        }

        else {
            console.warn("WebSocket not ready, message not sent:", msg);
        }
    }

    public onMessage(id: string, handler: (msg: WebMessage) => void): void {
        this.messageHandlers.set(id, handler);
    }

    public disconnect(): void {

        if (this.reconnectTimeout) {
            clearTimeout(this.reconnectTimeout);
        }
        this.ws?.close();
    }
}
