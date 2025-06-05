/*
 * ESP32 Hydroponisch Systeem Controller
 * 
 * Copyright (C) 2024 AXISKOM
 * Website: https://axiskom.nl
 * 
 * Dit programma is vrije software: je mag het herdistribueren en/of wijzigen
 * onder de voorwaarden van de GNU General Public License zoals gepubliceerd door
 * de Free Software Foundation, ofwel versie 3 van de licentie, of
 * (naar jouw keuze) een latere versie.
 * 
 * Deze software is ontwikkeld als onderdeel van het AXISKOM kennisplatform
 * voor zelfredzaamheid en zelfvoorzienend leven.
 *
 * Web interface HTML/CSS/JavaScript
 */

#ifndef WEBUI_H
#define WEBUI_H

// De volledige HTML, CSS en JavaScript code voor de webinterface
// Deze constante wordt gebruikt in WebServer.cpp
const char* WEBUI_HTML = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <title id="pageTitle">Axiskom Hydroponisch Systeem Controller</title>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    body {
      font-family: Arial, sans-serif;
      margin: 0;
      padding: 20px;
      max-width: 800px;
      margin: 0 auto;
      color: #1b3a4b;
    }
    .card {
      background-color: #fafafa;
      border-radius: 5px;
      box-shadow: 0 2px 5px rgba(0,0,0,0.1);
      padding: 20px;
      margin-bottom: 20px;
    }
    h1 {
      color: #1b3a4b;
      font-size: 24px;
      margin-top: 0;
    }
    .status-indicator {
      display: inline-block;
      width: 15px;
      height: 15px;
      border-radius: 50%;
      margin-right: 10px;
    }
    .on {
      background-color: #25d026;
    }
    .off {
      background-color: #f42703;
    }
    table {
      width: 100%;
      border-collapse: collapse;
    }
    table, th, td {
      border: 1px solid #ddd;
    }
    th, td {
      padding: 10px;
      text-align: left;
    }
    th {
      background-color: #f2f2f2;
    }
    button {
      background-color: #d35400;
      color: white;
      border: none;
      padding: 10px 15px;
      border-radius: 4px;
      cursor: pointer;
      font-size: 14px;
      margin-right: 10px;
    }
    button:hover {
      background-color: #3c6e47;
    }
    button.danger {
      background-color: #f42703;
    }
    button.danger:hover {
      background-color: #cc2103;
    }
    button.success {
      background-color: #25d026;
    }
    button.success:hover {
      background-color: #1ea81f;
    }
    input, select {
      padding: 8px;
      border: 1px solid #ddd;
      border-radius: 4px;
      width: 100%;
    }
    table input {
      width: 80px;
      text-align: center;
    }
    .settings-row {
      margin-bottom: 10px;
    }
    .settings-label {
      display: inline-block;
      width: 180px;
      margin-bottom: 5px;
    }
    .tab {
      display: none;
    }
    .tab.active {
      display: block;
    }
    .tab-buttons {
      margin-bottom: 20px;
      display: flex;
      border-bottom: 1px solid #ddd;
    }
    .tab-button {
      background-color: #d35400;
      border: none;
      padding: 10px 20px;
      margin-right: 5px;
      cursor: pointer;
      border-radius: 5px 5px 0 0;
    }
    .tab-button:hover {
      background-color: #3c6e47;
    }
    .tab-button.active {
      background-color: #3c6e47;
      color: white;
    }
    .temperature {
      font-size: 36px;
      font-weight: bold;
      text-align: center;
      margin: 20px 0;
    }
    .settings-save {
      margin-top: 15px;
    }
    .override-section {
      display: flex;
      align-items: center;
      justify-content: space-between;
      padding: 10px 0;
    }
    .night-mode {
      margin-top: 10px;
      padding: 5px;
      border-radius: 4px;
      background-color: #34495e;
      color: white;
      text-align: center;
    }
    .current-time {
      text-align: center;
      margin: 10px 0;
      font-size: 14px;
      color: #7f8c8d;
    }
    .version-info {
      text-align: center;
      margin-top: 20px;
      font-size: 12px;
      color: #95a5a6;
    }
    .flow-indicator {
      display: inline-block;
      padding: 5px 10px;
      margin-top: 10px;
      border-radius: 4px;
    }
    .flow-ok {
      background-color: #25d026;
      color: white;
    }
    .flow-error {
      background-color: #f42703;
      color: white;
    }
    .slider {
      -webkit-appearance: none;
      width: 100%;
      height: 15px;
      border-radius: 5px;
      background: #d3d3d3;
      outline: none;
      opacity: 0.7;
      -webkit-transition: .2s;
      transition: opacity .2s;
      margin: 20px 0;
    }
    .slider:hover {
      opacity: 1;
    }
    .slider::-webkit-slider-thumb {
      -webkit-appearance: none;
      appearance: none;
      width: 25px;
      height: 25px;
      border-radius: 50%;
      background: #d35400;
      cursor: pointer;
    }
    .slider::-moz-range-thumb {
      width: 25px;
      height: 25px;
      border-radius: 50%;
      background: #d35400;
      cursor: pointer;
    }
    .brightness-value {
      text-align: center;
      font-size: 18px;
      margin-bottom: 10px;
    }
    @media (max-width: 600px) {
      .settings-label {
        width: 100%;
      }
      .override-section {
        flex-direction: column;
        align-items: flex-start;
      }
      .override-section button {
        margin-top: 10px;
      }
    }
  </style>

  </head>
<body>
<div class="card">
    <h1 id="systemName">Hydroponisch Systeem</h1>
    <div class="temperature"><span id="temp">--</span>°C</div>
    <div class="current-time" id="dateTime">Tijd laden...</div>
    <div>
      <span class="status-indicator off" id="pumpIndicator"></span>
      <span id="pumpStatus">Pomp status: Laden...</span>
    </div>
    <div id="overrideStatus" style="margin-top: 10px; color: #e74c3c; display: none;">
      Handmatige modus actief
    </div>
    <div id="nightMode" class="night-mode" style="display: none;">
      Nachtmodus actief
    </div>
    <div id="continuModusIndicator" class="night-mode" style="display: none; background-color: #d35400;">
      Continue modus actief (NFT/DFT)
    </div>
    <div id="intervalModusIndicator" class="night-mode" style="display: none; background-color: #d35400;">
      Interval modus actief (Hydro Toren)
    </div>
    
    <div id="flowSensorData" style="display: none;">
      <div style="display: flex; justify-content: center; margin-top: 10px;">
        <div style="text-align: center; margin: 0 10px;">
          <div style="font-size: 24px;"><span id="flowRate">--</span> L/min</div>
          <div style="font-size: 12px;">Huidige stroming</div>
        </div>
        <div style="text-align: center; margin: 0 10px;">
          <div style="font-size: 24px;"><span id="totalFlow">--</span> L</div>
          <div style="font-size: 12px;">Totaal volume</div>
        </div>
      </div>
      <div id="flowStatus" class="flow-indicator flow-ok" style="display: none;">
        Waterstroming OK
      </div>
    </div>
    
  </div>

  <div class="tab-buttons">
    <button class="tab-button active" onclick="showTab('control')">Bediening</button>
    <button class="tab-button" onclick="showTab('settings')">Instellingen</button>
    <button id="flow-tab-button" class="tab-button" onclick="showTab('flow')" style="display: none;">Flowsensor</button>
  </div>

  <div id="control-tab" class="tab active card">
    <h2>Handmatige bediening</h2>
    <div class="override-section">
      <div>
        <button id="overrideOnBtn" class="success" onclick="activateOverride(true)">Pomp AAN</button>
        <button id="overrideOffBtn" class="danger" onclick="activateOverride(false)">Pomp UIT</button>
      </div>
      <div>
        <button id="cancelOverrideBtn" onclick="cancelOverride()" style="display: none;">Terug naar automatisch</button>
      </div>
    </div>
  </div>

  <div id="settings-tab" class="tab card">
    <h2>Systeeminstellingen</h2>
    
    <div class="settings-row">
      <div class="settings-label">Systeemnaam:</div>
      <input type="text" id="systeemnaam" maxlength="31" value="">
    </div>
    
    <h3>Temperatuurgrenzen</h3>
    <div class="settings-row">
      <div class="settings-label">Lage temperatuurgrens:</div>
      <input type="number" id="temp_laag_grens" min="0" max="40" step="0.5"> °C
    </div>
    <div class="settings-row">
      <div class="settings-label">Hoge temperatuurgrens:</div>
      <input type="number" id="temp_hoog_grens" min="0" max="40" step="0.5"> °C
    </div>

    <h3>Systeem Type</h3>
    <div class="settings-row">
      <div class="settings-label">Pomp modus:</div>
      <select id="continuModus">
        <option value="false">Interval modus (voor Hydro Toren)</option>
        <option value="true">Continue modus (voor NFT/DFT)</option>
      </select>
    </div>
    <p style="margin-bottom: 10px; font-size: 14px; color: #666;">
      Kies 'Continue modus' voor NFT of DFT systemen waarbij de pomp continu moet draaien.
    </p>
    
    <h3>Pompcycli instellingen</h3>
    <p style="margin-bottom: 10px; font-size: 14px; color: #666;">Je kunt tijden instellen tot op 30 seconden nauwkeurig (0.5 minuten).</p>
    <table>
      <tr>
        <th>Temperatuurbereik</th>
        <th>AAN (minuten)</th>
        <th>UIT (minuten)</th>
      </tr>
      <tr>
        <td>Laag (< <span id="laag_display">18</span>°C)</td>
        <td><input type="number" id="temp_laag_aan" min="0" step="0.1"></td>
        <td><input type="number" id="temp_laag_uit" min="0" step="0.1"></td>
      </tr>
      <tr>
        <td>Midden (<span id="midden_min_display">18</span>-<span id="midden_max_display">25</span>°C)</td>
        <td><input type="number" id="temp_midden_aan" min="0" step="0.1"></td>
        <td><input type="number" id="temp_midden_uit" min="0" step="0.1"></td>
      </tr>
      <tr>
        <td>Hoog (> <span id="hoog_display">25</span>°C)</td>
        <td><input type="number" id="temp_hoog_aan" min="0" step="0.1"></td>
        <td><input type="number" id="temp_hoog_uit" min="0" step="0.1"></td>
      </tr>
      <tr>
        <td>Nacht</td>
        <td><input type="number" id="nacht_aan" min="0" step="0.1"></td>
        <td><input type="number" id="nacht_uit" min="0" step="0.1"></td>
      </tr>
    </table>
    
    <div class="settings-save">
      <button onclick="saveSettings()">Instellingen opslaan</button>
    </div>
  </div>

  <div id="flow-tab" class="tab card">
    <h2>Flowsensor Instellingen</h2>
    
    <div class="settings-row">
      <div class="settings-label">Minimale flow rate:</div>
      <input type="number" id="minFlowRate" min="0.1" max="30" step="0.1" value="1.0"> L/min
    </div>

    <div class="settings-row">
      <div class="settings-label">Pomp capaciteit:</div>
      <input type="number" id="pumpCapacityLPH" min="200" max="3000" step="50" value="1500"> L/h
    </div>
    <p style="margin-bottom: 10px; font-size: 12px; color: #666;">
      Voer de maximale capaciteit van je pomp in (staat meestal op de pomp of verpakking)
    </p>
    
    <div class="settings-row">
      <div class="settings-label">E-mail waarschuwingen:</div>
      <select id="flowAlertEnabled">
        <option value="true">Ingeschakeld</option>
        <option value="false">Uitgeschakeld</option>
      </select>
    </div>
    
    <div class="settings-row">
      <div class="settings-label">E-mail adres afzender:</div>
      <input type="email" id="emailUsername" maxlength="63" placeholder="jouw.email@gmail.com">
    </div>
    
    <div class="settings-row">
      <div class="settings-label">App-wachtwoord:</div>
      <input type="password" id="emailPassword" maxlength="63" placeholder="xxxx xxxx xxxx xxxx">
    </div>
    
    <div class="settings-row">
      <div class="settings-label">E-mail adres ontvanger:</div>
      <input type="email" id="emailRecipient" maxlength="63" placeholder="ontvanger@email.com">
    </div>
    
    <div class="settings-row" style="margin-top: 20px;">
      <button id="sendTestEmailBtn" onclick="sendTestEmail()">Test E-mail Versturen</button>
      <button onclick="resetFlowCounter()">Reset Flow Teller</button>
    </div>
    
    <div class="settings-save">
      <button onclick="saveFlowSettings()">Instellingen opslaan</button>
    </div>
  </div>

  <div class="version-info">
    ESP32 Hydroponisch Systeem Controller v1.2.0<br>
    <a href="https://axiskom.nl" target="_blank">Mogelijk gemaakt door Axiskom.nl</a></br>
    <a href="https://github.com/imoliamedia/AXISKOM-ESP32-Hydroponisch-Systeem-Controller" target="_blank">Axiskom GitHub Repository</a>
  </div>
  </body>
  </html>

  <script>
    // Globale variabelen
    let overrideActive = false;
    let statusRefreshInterval;
    let isNachtModus = false;
    
    // Pagina initialisatie
    document.addEventListener('DOMContentLoaded', function() {
      fetchStatus();
      fetchSettings();
      
      // Controleer beschikbaarheid van optionele modules
      checkModulesAvailable();
      
      // Start auto-refresh
      statusRefreshInterval = setInterval(fetchStatus, 5000);
      
    });
    
    
    // Tab functionaliteit
    function showTab(tabName) {
      document.querySelectorAll('.tab').forEach(tab => {
        tab.classList.remove('active');
      });
      document.querySelectorAll('.tab-button').forEach(button => {
        button.classList.remove('active');
      });
      
      document.getElementById(tabName + '-tab').classList.add('active');
      document.querySelector(`.tab-button[onclick="showTab('${tabName}')"]`).classList.add('active');
    }
    
    // Status ophalen
    function fetchStatus() {
      fetch('/api/status')
        .then(response => response.json())
        .then(data => {
          document.getElementById('temp').textContent = data.temperature.toFixed(1);
          document.getElementById('pumpStatus').textContent = `Pomp status: ${data.pumpState ? 'AAN' : 'UIT'}`;
          document.getElementById('pumpIndicator').className = 
            data.pumpState ? 'status-indicator on' : 'status-indicator off';
          
          // Update tijdsweergave
          document.getElementById('dateTime').textContent = data.currentDateTime;
          
          // Update nachtmodus status
          isNachtModus = data.isNightMode;
          document.getElementById('nightMode').style.display = 
            isNachtModus ? 'block' : 'none';

          // Update modus indicators
          document.getElementById('continuModusIndicator').style.display = 
            data.continuModus && !data.overrideActive ? 'block' : 'none';
          document.getElementById('intervalModusIndicator').style.display = 
            !data.continuModus && !data.overrideActive ? 'block' : 'none';
          
          // Override status bijwerken
          overrideActive = data.overrideActive;
          document.getElementById('overrideStatus').style.display = 
            overrideActive ? 'block' : 'none';
          document.getElementById('cancelOverrideBtn').style.display = 
            overrideActive ? 'inline-block' : 'none';
        })
        .catch(error => {
          console.error('Fout bij het ophalen van status:', error);
        });
    }

    // Update flowsensor status
    function updateFlowStatus(data) {
      // Controleer eerst of flow sensor is ingeschakeld
      if (!data.flow_sensor_enabled) {
        console.log('Flow sensor niet ingeschakeld, skip flow status update');
        return;
      }
      
      console.log('Updating flow status with data:', data);
      
      // Update flow rate en totaal volume
      const flowRateElement = document.getElementById('flowRate');
      const totalFlowElement = document.getElementById('totalFlow');
      
      if (flowRateElement && data.flowRate !== undefined) {
        flowRateElement.textContent = data.flowRate.toFixed(2);
      }
      
      if (totalFlowElement && data.totalFlowVolume !== undefined) {
        totalFlowElement.textContent = data.totalFlowVolume.toFixed(1);
      }
      
      // Update flow status indicator
      const flowStatus = document.getElementById('flowStatus');
      if (flowStatus) {
        flowStatus.style.display = 'block';
        
        // Controleer voor problemen
        if (data.noFlowDetected === true) {
          flowStatus.className = 'flow-indicator flow-error';
          flowStatus.textContent = 'WAARSCHUWING: Geen waterstroming gedetecteerd!';
        } else if (data.pumpState && data.flowRate <= 0) {
          flowStatus.className = 'flow-indicator flow-error';
          flowStatus.textContent = 'WAARSCHUWING: Geen waterstroming!';
        } else if (data.pumpState && data.flowRate > 0) {
          flowStatus.className = 'flow-indicator flow-ok';
          flowStatus.textContent = 'Waterstroming OK';
        } else if (!data.pumpState) {
          // Als pomp uit staat, verberg flow status
          flowStatus.style.display = 'none';
        }
      }
    }
      
    // Instellingen ophalen
    function fetchSettings() {
      fetch('/api/settings')
        .then(response => response.json())
        .then(data => {
          // Vul de formuliervelden in met omrekening naar minuten
          document.getElementById('systeemnaam').value = data.systeemnaam;
          document.getElementById('temp_laag_grens').value = data.temp_laag_grens;
          document.getElementById('temp_hoog_grens').value = data.temp_hoog_grens;
          document.getElementById('temp_laag_aan').value = (data.temp_laag_aan / 60).toFixed(1);
          document.getElementById('temp_laag_uit').value = (data.temp_laag_uit / 60).toFixed(1);
          document.getElementById('temp_midden_aan').value = (data.temp_midden_aan / 60).toFixed(1);
          document.getElementById('temp_midden_uit').value = (data.temp_midden_uit / 60).toFixed(1);
          document.getElementById('temp_hoog_aan').value = (data.temp_hoog_aan / 60).toFixed(1);
          document.getElementById('temp_hoog_uit').value = (data.temp_hoog_uit / 60).toFixed(1);
          document.getElementById('nacht_aan').value = (data.nacht_aan / 60).toFixed(1);
          document.getElementById('nacht_uit').value = (data.nacht_uit / 60).toFixed(1);
          
          // Update pagina titel en labels
          document.getElementById('pageTitle').textContent = data.systeemnaam;
          document.getElementById('systemName').textContent = data.systeemnaam;
          
          // Update temperatuur grenzen in labels
          document.getElementById('laag_display').textContent = data.temp_laag_grens;
          document.getElementById('midden_min_display').textContent = data.temp_laag_grens;
          document.getElementById('midden_max_display').textContent = data.temp_hoog_grens;
          document.getElementById('hoog_display').textContent = data.temp_hoog_grens;

          // Update continue modus selectie
          document.getElementById('continuModus').value = data.continuModus.toString();
        })
        .catch(error => {
          console.error('Fout bij het ophalen van instellingen:', error);
        });
    }
    
    // Flowsensor instellingen ophalen
    function fetchFlowSettings() {
      fetch('/api/flowsettings')
        .then(response => response.json())
        .then(data => {
          document.getElementById('minFlowRate').value = data.minFlowRate;
          document.getElementById('flowAlertEnabled').value = data.flowAlertEnabled.toString();
          document.getElementById('pumpCapacityLPH').value = data.pumpCapacityLPH || 1500;
          document.getElementById('emailUsername').value = data.emailUsername || '';
          document.getElementById('emailPassword').value = data.emailPassword || '';
          document.getElementById('emailRecipient').value = data.emailRecipient || '';
        })
        .catch(error => {
          console.error('Fout bij het ophalen van flowsensor instellingen:', error);
        });
    }

    // Controleer beschikbaarheid van optionele modules
function checkModulesAvailable() {
  fetch('/api/config')
    .then(response => response.json())
    .then(data => {
      console.log('Module configuratie ontvangen:', data);
      
      // Flow sensor tab en data tonen/verbergen
      if (data.flow_sensor_enabled) {
        console.log('Flowsensor is ingeschakeld, toon interface elementen');
        
        // Toon flow sensor tab knop
        const flowTabButton = document.getElementById('flow-tab-button');
        if (flowTabButton) {
          flowTabButton.style.display = 'block';
        }
        
        // Toon flow sensor data sectie op hoofdpagina
        const flowSensorData = document.getElementById('flowSensorData');
        if (flowSensorData) {
          flowSensorData.style.display = 'block';
        }
        
        // Laad flow sensor instellingen
        fetchFlowSettings();
        
        // Update status met flow data
        fetchStatusWithFlow();
      } else {
        console.log('Flowsensor is uitgeschakeld, verberg interface elementen');
        
        // Verberg flow sensor tab knop
        const flowTabButton = document.getElementById('flow-tab-button');
        if (flowTabButton) {
          flowTabButton.style.display = 'none';
        }
        
        // Verberg flow sensor data sectie
        const flowSensorData = document.getElementById('flowSensorData');
        if (flowSensorData) {
          flowSensorData.style.display = 'none';
        }
      }
      
      // E-mail functionaliteit (alleen als flow sensor ook is ingeschakeld)
      if (data.email_notification_enabled) {
        const sendTestEmailBtn = document.getElementById('sendTestEmailBtn');
        if (sendTestEmailBtn) {
          sendTestEmailBtn.style.display = 'inline-block';
        }
      } else {
        const sendTestEmailBtn = document.getElementById('sendTestEmailBtn');
        if (sendTestEmailBtn) {
          sendTestEmailBtn.style.display = 'none';
        }
      }
    })
    .catch(error => {
      console.error('Fout bij het ophalen van module configuratie:', error);
    });
}

// Aangepaste fetchStatus functie die ook flow data update
function fetchStatusWithFlow() {
  fetch('/api/status')
    .then(response => response.json())
    .then(data => {
      // Normale status updates
      document.getElementById('temp').textContent = data.temperature.toFixed(1);
      document.getElementById('pumpStatus').textContent = `Pomp status: ${data.pumpState ? 'AAN' : 'UIT'}`;
      document.getElementById('pumpIndicator').className = 
        data.pumpState ? 'status-indicator on' : 'status-indicator off';
      
      // Update tijdsweergave
      document.getElementById('dateTime').textContent = data.currentDateTime;
      
      // Update nachtmodus status
      isNachtModus = data.isNightMode;
      document.getElementById('nightMode').style.display = 
        isNachtModus ? 'block' : 'none';

      // Update modus indicators
      document.getElementById('continuModusIndicator').style.display = 
        data.continuModus && !data.overrideActive ? 'block' : 'none';
      document.getElementById('intervalModusIndicator').style.display = 
        !data.continuModus && !data.overrideActive ? 'block' : 'none';
      
      // Override status bijwerken
      overrideActive = data.overrideActive;
      document.getElementById('overrideStatus').style.display = 
        overrideActive ? 'block' : 'none';
      document.getElementById('cancelOverrideBtn').style.display = 
        overrideActive ? 'inline-block' : 'none';
      
      // **BELANGRIJK: Update flow sensor data indien beschikbaar**
      if (data.flow_sensor_enabled) {
        updateFlowStatus(data);
      }
    })
    .catch(error => {
      console.error('Fout bij het ophalen van status:', error);
    });
}
    

    // Helper functie om de dichtstbijzijnde optie te selecteren
    function setClosestOptionByValue(selectElement, targetValue) {
      let closestOption = null;
      let closestDistance = Infinity;
      
      for (let i = 0; i < selectElement.options.length; i++) {
        const optionValue = parseInt(selectElement.options[i].value);
        const distance = Math.abs(optionValue - targetValue);
        
        if (distance < closestDistance) {
          closestDistance = distance;
          closestOption = i;
        }
      }
      
      if (closestOption !== null) {
        selectElement.selectedIndex = closestOption;
      }
    }

    // Instellingen opslaan
    function saveSettings() {
      const settings = {
        systeemnaam: document.getElementById('systeemnaam').value,
        temp_laag_grens: parseFloat(document.getElementById('temp_laag_grens').value),
        temp_hoog_grens: parseFloat(document.getElementById('temp_hoog_grens').value),
        temp_laag_aan: Math.round(parseFloat(document.getElementById('temp_laag_aan').value) * 60),
        temp_laag_uit: Math.round(parseFloat(document.getElementById('temp_laag_uit').value) * 60),
        temp_midden_aan: Math.round(parseFloat(document.getElementById('temp_midden_aan').value) * 60),
        temp_midden_uit: Math.round(parseFloat(document.getElementById('temp_midden_uit').value) * 60),
        temp_hoog_aan: Math.round(parseFloat(document.getElementById('temp_hoog_aan').value) * 60),
        temp_hoog_uit: Math.round(parseFloat(document.getElementById('temp_hoog_uit').value) * 60),
        nacht_aan: Math.round(parseFloat(document.getElementById('nacht_aan').value) * 60),
        nacht_uit: Math.round(parseFloat(document.getElementById('nacht_uit').value) * 60),
        continuModus: document.getElementById('continuModus').value === 'true'
      };
      
      fetch('/api/settings', {
        method: 'POST',
        headers: {
          'Content-Type': 'application/json',
        },
        body: JSON.stringify(settings)
      })
      .then(response => {
        if (!response.ok) {
          return response.text().then(text => { throw new Error(text) });
        }
        return response.json();
      })
      .then(data => {
        alert('Instellingen opgeslagen!');
        fetchSettings(); // Refresh instellingen
      })
      .catch(error => {
        console.error('Fout bij het opslaan van instellingen:', error);
        alert('Fout bij het opslaan van instellingen: ' + error.message);
      });
    }
    
    // Flowsensor instellingen opslaan
    function saveFlowSettings() {
      const settings = {
        minFlowRate: parseFloat(document.getElementById('minFlowRate').value),
        flowAlertEnabled: document.getElementById('flowAlertEnabled').value === 'true',
        pumpCapacityLPH: parseInt(document.getElementById('pumpCapacityLPH').value),
        emailUsername: document.getElementById('emailUsername').value,
        emailPassword: document.getElementById('emailPassword').value,
        emailRecipient: document.getElementById('emailRecipient').value
      };
      
      fetch('/api/flowsettings', {
        method: 'POST',
        headers: {
          'Content-Type': 'application/json',
        },
        body: JSON.stringify(settings)
      })
      .then(response => {
        if (!response.ok) {
          return response.text().then(text => { throw new Error(text) });
        }
        return response.json();
      })
      .then(data => {
        alert('Flowsensor instellingen opgeslagen!');
      })
      .catch(error => {
        console.error('Fout bij het opslaan van flowsensor instellingen:', error);
        alert('Fout bij het opslaan van flowsensor instellingen: ' + error.message);
      });
    }
    
    
    // Test e-mail versturen
    function sendTestEmail() {
      fetch('/api/testemail', {
        method: 'POST'
      })
      .then(response => response.json())
      .then(data => {
        if (data.status === 'success') {
          alert('Test e-mail succesvol verzonden!');
        } else {
          alert('Fout bij het versturen van test e-mail: ' + data.message);
        }
      })
      .catch(error => {
        console.error('Fout bij het versturen van test e-mail:', error);
        alert('Fout bij het versturen van test e-mail');
      });
    }
    
    // Reset flow counter
    function resetFlowCounter() {
      fetch('/api/resetflow', {
        method: 'POST'
      })
      .then(response => response.json())
      .then(data => {
        alert('Flowsensor teller gereset!');
        fetchStatus(); // Ververs status om nieuwe tellerstand weer te geven
      })
      .catch(error => {
        console.error('Fout bij het resetten van flowsensor teller:', error);
        alert('Fout bij het resetten van flowsensor teller');
      });
    }
    
    // Override activeren
    function activateOverride(pumpOn) {
      fetch('/api/override', {
        method: 'POST',
        headers: {
          'Content-Type': 'application/json',
        },
        body: JSON.stringify({
          active: true,
          pumpState: pumpOn
        })
      })
      .then(response => response.json())
      .then(data => {
        fetchStatus(); // Status updaten
      })
      .catch(error => {
        console.error('Fout bij het activeren van override:', error);
      });
    }
    
    // Override annuleren
    function cancelOverride() {
      fetch('/api/override', {
        method: 'POST',
        headers: {
          'Content-Type': 'application/json',
        },
        body: JSON.stringify({
          active: false
        })
      })
      .then(response => response.json())
      .then(data => {
        fetchStatus(); // Status updaten
      })
      .catch(error => {
        console.error('Fout bij het annuleren van override:', error);
      });
    }
    
  </script>

)rawliteral";

#endif // WEBUI_H