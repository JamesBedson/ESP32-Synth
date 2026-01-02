export abstract class UIControl extends HTMLElement {

  protected _installed = false;

  connectedCallback() {
    if (this._installed) return;
    this._installed = true;
    this.installBehaviour();
  }

  protected installBehaviour(): void {
    // optional override
  }

  bind(_param: unknown): void {
    // optional override
  }
}
