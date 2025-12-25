export type ParamKind = "float" | "int" | "bool";
export type ParamUI   = "knob" | "slider" | "toggle" | "button";

export class AudioParameter<T> {
  private __brand!: "AudioParameter";

  readonly id: string;
  readonly min: number;
  readonly max: number;
  readonly kind: ParamKind;

  // 🟦 Optional UI metadata
  readonly label?: string;
  readonly unit?: string;
  readonly ui?: ParamUI;
  readonly step?: number;

  private _value: number;

  private constructor(
    id: string,
    def: number,
    min: number,
    max: number,
    kind: ParamKind,
    meta?: {
      label?: string;
      unit?: string;
      ui?: ParamUI;
      step?: number;
    }
  ) {
    this.id = id;
    this._value = def;
    this.min = min;
    this.max = max;
    this.kind = kind;

    if (meta) {
    if (meta.label !== undefined) this.label = meta.label;
    if (meta.unit  !== undefined) this.unit  = meta.unit;
    if (meta.ui    !== undefined) this.ui    = meta.ui;
    if (meta.step  !== undefined) this.step  = meta.step;
    }
  }

  static __make<T>(
    id: string,
    def: number,
    min: number,
    max: number,
    kind: ParamKind,
    meta?: {
      label?: string;
      unit?: string;
      ui?: ParamUI;
      step?: number;
    }
  ): AudioParameter<T> {
    return new AudioParameter<T>(id, def, min, max, kind, meta);
  }

  // ======================
  // Typed access
  // ======================

  get(): T {
    switch (this.kind) {
      case "bool":  return (this._value !== 0) as unknown as T;
      case "int":   return Math.round(this._value) as unknown as T;
      default:      return this._value as unknown as T;
    }
  }

  set(v: T) {
    switch (this.kind) {
      case "bool": this._value = (v as unknown as boolean) ? 1 : 0; break;
      case "int":  this._value = Math.round(v as unknown as number); break;
      default:     this._value = v as unknown as number;
    }
  }

  // ======================
  // DSP-compatible helpers
  // ======================

  getAsFloat(): number {
    return this._value;
  }

  setFromFloat(v: number) {
    this._value = Math.min(this.max, Math.max(this.min, v));
  }

  getNormalised(): number {
    return (this._value - this.min) / (this.max - this.min);
  }

  setNormalised(n: number) {
    this.setFromFloat(this.min + n * (this.max - this.min));
  }
}
