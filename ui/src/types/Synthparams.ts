import { AudioParameter } from "./AudioParameter";

export class Synthparams {
  amplitude = AudioParameter.__make<number>("amplitude", 0.5, 0.0, 1.0, "float", undefined);
  frequency = AudioParameter.__make<number>("frequency", 500.0, 20.0, 15000.0, "float", undefined);
}