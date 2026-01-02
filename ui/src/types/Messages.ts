export type WebMessageType = "set" | "get" | "cmd" | "param";

export type WebMessage = {
  type: WebMessageType;
  id?: string;
  value?: number;
  command?: string;
  args?: string;
};

