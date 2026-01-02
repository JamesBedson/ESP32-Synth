export type ParamKind = "float" | "int" | "bool";
export type ParamUI   = "knob" | "slider" | "toggle" | "button";
type ParamListener<T> = (value: T) => void;

export class AudioParameter<T> {
  private __brand!: "AudioParameter";

  readonly id: string;
  readonly min: number;
  readonly max: number;
  readonly kind: ParamKind;

  // Optional UI metadata
  readonly label?: string;
  readonly unit?: string;
  readonly ui?: ParamUI;
  readonly step?: number;

  private _value: number;
  private _listeners: Set<ParamListener<T>> = new Set();

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
    const before = this._value;

    switch (this.kind) {
      case "bool": this._value = (v as unknown as boolean) ? 1 : 0; break;
      case "int":  this._value = Math.round(v as unknown as number); break;
      default:     this._value = v as unknown as number;
    }

    if (this._value !== before) this._emit();
  }

  onChange(cb: ParamListener<T>): () => void {
    this._listeners.add(cb);
    return () => this._listeners.delete(cb);
  }

  private _emit() {
    const v = this.get(); // typed value
    for (const cb of this._listeners) cb(v);
  }

  // ======================
  // DSP-compatible helpers
  // ======================

  getAsFloat(): number {
    return this._value;
  }

  setFromFloat(v: number) {
    const before = this._value;
    this._value = Math.min(this.max, Math.max(this.min, v));
    if (this._value !== before) this._emit();
  }

  getNormalised(): number {
    return (this._value - this.min) / (this.max - this.min);
  }

  setNormalised(n: number) {
    const before = this._value;
    this.setFromFloat(this.min + n * (this.max - this.min));
    if (this._value !== before) this._emit();
  }
}
