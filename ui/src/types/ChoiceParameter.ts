import { AudioParameter } from "./AudioParameter";

// Wrapper for enum-like string choices backed by AudioParameter<number>.
export class ChoiceParameter<ChoiceT extends string> {
  private param: AudioParameter<number>;
  readonly choices: readonly ChoiceT[];

  private constructor(param: AudioParameter<number>, choices: readonly ChoiceT[]) {
    this.param = param;
    this.choices = choices;
  }

  static __make<ChoiceT extends string>(
    id: string,
    def: number,
    min: number,
    max: number,
    kind: "int",
    meta: {
      label?: string;
      unit?: string;
      ui?: string;
      step?: number;
    } | undefined,
    choices: readonly ChoiceT[]
  ): ChoiceParameter<ChoiceT> {
    const param = AudioParameter.__make<number>(id, def, min, max, kind, meta);
    return new ChoiceParameter<ChoiceT>(param, choices);
  }

  get(): ChoiceT {
    const idx = this.param.get();
    return this.choices[idx] ?? this.choices[0];
  }

  set(value: ChoiceT): void {
    const idx = this.choices.indexOf(value);
    if (idx >= 0) {
      this.param.set(idx);
    }
  }

  getIndex(): number {
    return this.param.get();
  }

  setIndex(index: number): void {
    this.param.set(index);
  }

  raw(): AudioParameter<number> {
    return this.param;
  }
}
