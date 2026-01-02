import { UIControl } from "../UIControl";
import { AudioParameter } from "../../types/AudioParameter";
import { ParamBinder } from "../ParameterBinder";
import "./slider.css"

export class VerticalSlider extends UIControl {
  private slider!: HTMLInputElement;
  private _value = 0;
  private binder = new ParamBinder<number>();

  protected installBehaviour(): void {
    this.slider = document.createElement("input");
    this.slider.type = "range";
    this.slider.min = "0";
    this.slider.max = "1";
    this.slider.step = "0.001";
    this.append(this.slider);

    this.slider.addEventListener("input", () => {
      const n = parseFloat(this.slider.value);
      this._value = n;
      this.reflect();

      this.dispatchEvent(new CustomEvent("change", {
        bubbles: true,
        detail: { value: n }
      }));
    });
  }

  public bind(param: AudioParameter<number>): void {
    this.binder.bind(
      param,
      _ => this.setFromParam(param),
      handler => {
        const fn = (e: any) => handler(e.detail.value);
        this.addEventListener("change", fn);
        return () => this.removeEventListener("change", fn);
      }
    );
  }

  private setFromParam(param: AudioParameter<number>) {
    const val = param.getNormalised();
    if (this._value === val) return;
    this._value = val;
    this.reflect();
  }

  private reflect() {
    this.slider.value = this._value.toString();
  }
}

customElements.define("vertical-slider", VerticalSlider);