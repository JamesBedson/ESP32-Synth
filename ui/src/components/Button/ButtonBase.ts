import { UIControl } from "../UIControl";

export abstract class ButtonBase extends UIControl {
  protected button!: HTMLButtonElement;

  static get observedAttributes() {
    return ["label", "disabled"];
  }

  constructor() {
    super();
    this.button = document.createElement("button");
    this.button.type = "button";
    this.append(this.button);
  }

  protected installBehaviour(): void {
    this.sync();
    this.installButtonBehavior();
  }

  protected abstract installButtonBehavior(): void;

  attributeChangedCallback() {
    this.sync();
  }

  protected sync() {
    this.button.textContent = this.label;
    this.button.disabled = this.disabled;
  }

  protected get label() {
    return this.getAttribute("label") ?? "";
  }

  protected get disabled() {
    return this.hasAttribute("disabled");
  }
}
