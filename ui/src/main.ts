document.documentElement.dataset.theme = "dark";
import './styles/global.css';

import './components/Slider/VerticalSlider';
import './components/ToggleButton/ToggleButton';

// Services
import { WebSocketService } from './services/WebSocketService';
import { WebSocketParameterTransport } from './services/WebSocketParameterTransport';
import { LocalParamTransport } from './services/LocalParameterTransport';

// UI Components
import type { ToggleButton } from './components/ToggleButton/ToggleButton';
import type { VerticalSlider } from './components/Slider/VerticalSlider';

// AudioParameters
import { TestParams } from './types/TestParams';
// ------------------------------------------------------------
const app = document.getElementById('app')!;
app.innerHTML = `
  <h1>ESPSynth Test</h1>

  <div class="row">
    <label>Enabled</label>
    <toggle-button id="enabledToggle"></toggle-button>
  </div>

  <div class="row">
    <label>Amplitude</label>
    <vertical-slider id="slider"></vertical-slider>
  </div>
`;

// Parameter <-> WebSocket Binding
const isLocal = location.hostname === "localhost";
const params = TestParams.instance();
(window as any).params = params;

if (isLocal) {
  const local = new LocalParamTransport();
  local.bind(params);
}

else {
  const ws = new WebSocketService("ws://" + location.hostname + "/ws");
  const transport = new WebSocketParameterTransport(ws);
  transport.bind(params);
}

// Parameter <-> UI Binding
const enabledToggle = document.getElementById("enabledToggle") as ToggleButton;
enabledToggle.bind(params.enabled);

const slider = document.getElementById("slider") as VerticalSlider;
slider.bind(params.amplitude);