from flask import Flask, request, jsonify
import requests

app = Flask(__name__)

# Configurações do Blynk
BLYNK_AUTH = ""  # Token de autenticação do Blynk
PIN_PH = "V0"                  # Pino virtual para o valor de pH
PIN_TEMP = "V1"                # Pino virtual para o valor de temperatura
PIN_LED_GREEN = "V2"
PIN_LED_RED = "V3"
BLYNK_BASE_URL = "https://blynk.cloud/external/api/update"  # URL da API do Blynk


@app.route('/data', methods=['POST'])
def receive_data():
    """
    Endpoint para receber dados de pH e temperatura e enviar para o Blynk IoT.
    
    Espera um JSON no formato:
    {
        "ph": valor_ph,
        "temperatura": valor_temperatura,
        "aviso": valor_aviso
    }
    
    Retorna:
    - 200 OK se os dados foram processados e enviados
    - 400 Bad Request se houver erro na requisição
    """
    if not request.is_json:
        return jsonify({"error": "Requisição deve ser em JSON."}), 400
    
    try:
        data = request.get_json()
        ph_value = data.get('ph')
        temp_value = data.get('temperatura')
        alert_value = data.get('aviso')

        if ph_value is None or temp_value is None or alert_value is None:
            return jsonify({"error": "Dados de pH, temperatura ou aviso ausentes."}), 400

        print(f"Dados recebidos: pH = {ph_value}, Temperatura = {temp_value}, Aviso = {bool(alert_value)}")

        # Envia pH para o Blynk
        response_ph = requests.get(
            f"{BLYNK_BASE_URL}?token={BLYNK_AUTH}&{PIN_PH}={ph_value}"
        )
        
        # Envia temperatura para o Blynk
        response_temp = requests.get(
            f"{BLYNK_BASE_URL}?token={BLYNK_AUTH}&{PIN_TEMP}={temp_value}"
        )

        # Envia aviso para o Blynk
        if(alert_value):
            requests.get(f"{BLYNK_BASE_URL}?token={BLYNK_AUTH}&{PIN_LED_RED}=0")
            response_alert = requests.get(f"{BLYNK_BASE_URL}?token={BLYNK_AUTH}&{PIN_LED_GREEN}=1")
        else:
            requests.get(f"{BLYNK_BASE_URL}?token={BLYNK_AUTH}&{PIN_LED_GREEN}=0")
            response_alert = requests.get(f"{BLYNK_BASE_URL}?token={BLYNK_AUTH}&{PIN_LED_RED}=1")

        return jsonify({
            "message": "Dados enviados ao Blynk!",
            "blynk_response_ph": response_ph.text,
            "blynk_response_temp": response_temp.text,
            "blynk_response_alert": response_alert.text
        }), 200

    except Exception as e:
        return jsonify({"error": f"Erro ao processar JSON: {str(e)}"}), 400


if __name__ == '__main__':
    app.run(debug=True, host='0.0.0.0')
