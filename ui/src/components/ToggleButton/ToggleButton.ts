import { ButtonBase } from "../Button/ButtonBase";
import { AudioParameter } from "../../types/AudioParameter";
import { ParamBinder } from "../ParameterBinder";
import "./toggle-button.css";

export class ToggleButton extends ButtonBase {

  private _on = false;
  private binder = new ParamBinder<boolean>();

  protected installButtonBehavior() {
    
    this.button.innerHTML = `
      <div class="track">
        <div class="thumb"></div>
      </div>
    `;

    this.button.addEventListener("click", () => {
      if (this.disabled) return;

      this._on = !this._on;
      this.reflect();

      this.dispatchEvent(
        new CustomEvent("toggle", {
          bubbles: true,
          detail: { value: this._on },
        })
      );
    });

    this.reflect();
  }

  // ----------------------------
  // Public control API
  // ----------------------------

  setOn(value: boolean) {
    if (this._on === value) return;
    this._on = value;
    this.reflect();
  }

  isOn(): boolean {
    return this._on;
  }

  // ----------------------------
  // Parameter binding
  // ----------------------------

  bind(param: AudioParameter<boolean>) {
    this.binder.bind(
      param,
      v => this.setOn(v),
      handler => {
        const fn = (e: any) => handler(e.detail.value);
        this.addEventListener("toggle", fn);
        return () => this.removeEventListener("toggle", fn);
      }
    );
  }

  // ----------------------------
  // Internal rendering
  // ----------------------------

  private reflect() {
    this.toggleAttribute("on", this._on);
  }
}

customElements.define("toggle-button", ToggleButton);
