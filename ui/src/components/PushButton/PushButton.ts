import { ButtonBase } from "../Button/ButtonBase";
import "./push-button.css";

export class PushButton extends ButtonBase {
  protected installBehavior() {
    this.button.addEventListener("click", () => {
      if (this.disabled) return;
      this.dispatchEvent(new CustomEvent("push", { bubbles: true }));
    });
  }
}

customElements.define("push-button", PushButton);
