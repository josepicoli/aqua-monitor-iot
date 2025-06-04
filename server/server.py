from flask import Flask, request, jsonify
import requests
import smtplib

app = Flask(__name__)

# Configurações do Blynk
BLYNK_AUTH = ""  # Token de autenticação do Blynk
PIN_PH = "V0"                  # Pino virtual para o valor de pH
PIN_TEMP = "V1"                # Pino virtual para o valor de temperatura
PIN_LED_GREEN = "V2"
PIN_LED_RED = "V3"
BLYNK_BASE_URL = "https://blynk.cloud/external/api/update"  # URL da API do Blynk

def send_to_email(destination_email, msg):
    smtpObj = smtplib.SMTP('smtp.gmail.com', 587)
    smtpObj.ehlo()
    smtpObj.starttls()
    user = ""
    password = ""
    smtpObj.login(user, password)
    for i in destination_email:
        smtpObj.sendmail(user, i, f'Subject: Aviso\n{msg}')
    smtpObj.quit()

@app.route('/data', methods=['POST'])
def receive_data():
    """
    Endpoint para receber dados de pH e temperatura e enviar para o Blynk IoT.
    
    Espera um JSON no formato:
    {
        "ph": valor_ph,
        "temperatura": valor_temperatura,
        "aviso": valor_aviso,
        "msg": valor_msg
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
        msg_value = data.get('msg')

        if ph_value is None or temp_value is None or alert_value is None or msg_value is None:
            return jsonify({"error": "Dados de pH, temperatura, aviso ou msg ausentes."}), 400

        print(f"Dados recebidos: pH = {ph_value}, Temperatura = {temp_value}, Aviso = {bool(alert_value)}, msg = {msg_value}")

        # Envia pH para o Blynk
        response_ph = requests.get(
            f"{BLYNK_BASE_URL}?token={BLYNK_AUTH}&{PIN_PH}={ph_value}"
        )
        
        # Envia temperatura para o Blynk
        response_temp = requests.get(
            f"{BLYNK_BASE_URL}?token={BLYNK_AUTH}&{PIN_TEMP}={temp_value}"
        )

        # Envia aviso para o Blynk
        if(int(alert_value)):
            requests.get(f"{BLYNK_BASE_URL}?token={BLYNK_AUTH}&{PIN_LED_RED}=0")
            response_alert = requests.get(f"{BLYNK_BASE_URL}?token={BLYNK_AUTH}&{PIN_LED_GREEN}=1")
        else:
            requests.get(f"{BLYNK_BASE_URL}?token={BLYNK_AUTH}&{PIN_LED_GREEN}=0")
            response_alert = requests.get(f"{BLYNK_BASE_URL}?token={BLYNK_AUTH}&{PIN_LED_RED}=1")

        # Enviar aviso para o email
        if(not int(alert_value)):
            send_to_email(["...@gmail.com"], msg_value)

        return jsonify({
            "message": "Dados enviados ao Blynk!",
            "blynk_response_ph": response_ph.text,
            "blynk_response_temp": response_temp.text,
            "blynk_response_alert": response_alert.text
        }), 200    
    
    except Exception as e:
        print(f"Erro ao processar dados: {e}")
        return jsonify({"error": f"Erro ao processar JSON: {str(e)}"}), 400


if __name__ == '__main__':
    app.run(debug=True, host='0.0.0.0')
