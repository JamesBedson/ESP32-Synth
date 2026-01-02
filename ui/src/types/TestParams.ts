import { AudioParameter } from "./AudioParameter";
import { ChoiceParameter } from "./ChoiceParameter";

export class TestParams {
  private static _instance: TestParams | null = null;

  static instance(): TestParams {
    if (!TestParams._instance) {
      TestParams._instance = new TestParams();
    }
    return TestParams._instance;
  }

  private constructor() {}

  static readonly waveformChoices = ["Sine", "Saw", "Square", "Triangle", "Noise"] as const;

  amplitude = AudioParameter.__make<number>("synth.amplitude", 0.5, 0.0, 1.0, "float", { label: "Amplitude", unit: "", ui: "slider", step: 0.01 });
  frequency = AudioParameter.__make<number>("synth.frequency", 440.0, 20.0, 15000.0, "float", { label: "Frequency", unit: "Hz", ui: "knob", step: 1.0 });
  enabled = AudioParameter.__make<boolean>("synth.enabled", 1, 0, 1, "bool", { label: "Enabled", ui: "toggle" });
  waveform = ChoiceParameter.__make("synth.waveform", 1, 0, 4, "int", { label: "Waveform", ui: "select" }, TestParams.waveformChoices);

  public all(): Array<AudioParameter<any>> {
    return [
      this.amplitude,
      this.frequency,
      this.enabled,
      this.waveform.raw(),
    ];
  }
}