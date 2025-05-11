from flask import Flask, request, jsonify
import requests

app = Flask(__name__)

# Configurações do Blynk
BLYNK_AUTH = "SEU_TOKEN_AQUI"  # Token de autenticação do Blynk
PIN_PH = "V0"                  # Pino virtual para o valor de pH
PIN_TEMP = "V1"                # Pino virtual para o valor de temperatura
BLYNK_BASE_URL = "https://blynk.cloud/external/api/update"  # URL da API do Blynk


@app.route('/data', methods=['POST'])
def receive_data():
    """
    Endpoint para receber dados de pH e temperatura e enviar para o Blynk IoT.
    
    Espera um JSON no formato:
    {
        "ph": valor_ph,
        "temperatura": valor_temperatura
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

        if ph_value is None or temp_value is None:
            return jsonify({"error": "Dados de pH ou temperatura ausentes."}), 400

        print(f"Dados recebidos: pH = {ph_value}, Temperatura = {temp_value}")

        # Envia pH para o Blynk
        response_ph = requests.get(
            f"{BLYNK_BASE_URL}?token={BLYNK_AUTH}&{PIN_PH}={ph_value}"
        )
        
        # Envia temperatura para o Blynk
        response_temp = requests.get(
            f"{BLYNK_BASE_URL}?token={BLYNK_AUTH}&{PIN_TEMP}={temp_value}"
        )

        return jsonify({
            "message": "Dados enviados ao Blynk!",
            "blynk_response_ph": response_ph.text,
            "blynk_response_temp": response_temp.text
        }), 200

    except Exception as e:
        return jsonify({"error": f"Erro ao processar JSON: {str(e)}"}), 400


if __name__ == '__main__':
    app.run(debug=True, host='0.0.0.0')
