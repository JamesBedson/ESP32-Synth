import type { AudioParameter } from "../types/AudioParameter";

export class LocalParamTransport {
  bind(paramSet: { all(): AudioParameter<any>[] }) {
    for (const p of paramSet.all()) {
      p.onChange(value => {
        console.log(`[LOCAL TRANSPORT] ${p.id} =`, value);
      });
    }
  }
}
