import { AudioParameter } from "./AudioParameter";

export class SynthParams {
  private static _instance: SynthParams | null = null;

  static instance(): SynthParams {
    if (!SynthParams._instance) {
      SynthParams._instance = new SynthParams();
    }
    return SynthParams._instance;
  }

  private constructor() {}

  amplitude = AudioParameter.__make<number>("synth.amplitude", 0.5, 0.0, 1.0, "float", undefined);
  frequency = AudioParameter.__make<number>("synth.frequency", 440.0, 20.0, 15000.0, "float", undefined);
}