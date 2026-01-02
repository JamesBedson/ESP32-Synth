import { AudioParameter } from "../types/AudioParameter";

export class ParamBinder<T> {
  private _unsubscribe?: () => void;
  private _param?: AudioParameter<T>;

  bind(
    param: AudioParameter<T>,
    onParamChange: (v: T) => void,
    onUIChange: (handler: (v: T) => void) => () => void
  ) {
    this._unsubscribe?.();
    this._param = param;

    // Param → UI
    onParamChange(param.get());

    // UI → Param
    const offUI = onUIChange(v => param.set(v));

    // Param → UI (live updates)
    this._unsubscribe = param.onChange(v => onParamChange(v));

    return () => {
      offUI();
      this._unsubscribe?.();
    };
  }
}
