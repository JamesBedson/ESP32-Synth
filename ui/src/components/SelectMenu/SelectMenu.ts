import "./select-menu.css";
import type { ChoiceParameter } from "../../types/ChoiceParameter";

export class SelectMenu extends HTMLElement {
  private button!: HTMLButtonElement;
  private list!: HTMLDivElement;
  private label!: HTMLSpanElement;

  private param?: ChoiceParameter<any>;

  connectedCallback() {
    this.innerHTML = `
      <button class="control">
        <span class="label"></span>
        <span class="arrow">▼</span>
      </button>
      <div class="menu"></div>
    `;

    this.button = this.querySelector(".control")!;
    this.list = this.querySelector(".menu")!;
    this.label = this.querySelector(".label")!;

    this.button.addEventListener("click", () => this.toggle());
    document.addEventListener("click", e => {
      if (!this.contains(e.target as Node)) this.close();
    });
  }

  bind<ChoiceT extends string>(param: ChoiceParameter<ChoiceT>) {
    this.param = param;
    this.buildOptions(param);
    this.syncFromParam();
  }

  private buildOptions<ChoiceT extends string>(param: ChoiceParameter<ChoiceT>) {
    this.list.innerHTML = "";

    param.choices.forEach(choice => {
      const item = document.createElement("div");
      item.className = "item";
      item.textContent = choice;

      item.addEventListener("click", () => {
        param.set(choice);
        this.syncFromParam();
        this.close();
      });

      this.list.append(item);
    });
  }

  private syncFromParam() {
    if (!this.param) return;
    this.label.textContent = this.param.get();
  }

  private toggle() {
    this.classList.toggle("open");
  }

  private close() {
    this.classList.remove("open");
  }
}

customElements.define("select-menu", SelectMenu);
