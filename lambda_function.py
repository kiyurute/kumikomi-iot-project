import os
import sys
import json
import boto3
import logging

logger = logging.getLogger()
logger.setLevel(logging.INFO)

print('Loading function') 

iot = boto3.client('iot-data')

from linebot import (
    LineBotApi, WebhookHandler
)
from linebot.models import (
    MessageEvent, TextMessage, TextSendMessage,
)
from linebot.exceptions import (
    LineBotApiError, InvalidSignatureError
)

channel_secret = os.getenv('LINE_CHANNEL_SECRET', None)
channel_access_token = os.getenv('LINE_CHANNEL_ACCESS_TOKEN', None)
if channel_secret is None:
    logger.error('Specify LINE_CHANNEL_SECRET as environment variable.')
    sys.exit(1)
if channel_access_token is None:
    logger.error('Specify LINE_CHANNEL_ACCESS_TOKEN as environment variable.')
    sys.exit(1)

line_bot_api = LineBotApi(channel_access_token)
handler = WebhookHandler(channel_secret)

statusNum=0

def lambda_handler(event, context):
    logger.info(event)
    
    topic = '$aws/things/MyESP32/shadow/update/delta'
    payload = {"a":"a"}
    # iot.publish(
    #         topic=topic,
    #         qos=0,
    #         payload=json.dumps(payload, ensure_ascii=False)
    #     )
        
    signature = event["headers"]["X-Line-Signature"]
    body = event["body"]
    ok_json = {"isBase64Encoded": False,
               "statusCode": 200,
               "headers": {},
               "body": ""}
    error_json = {"isBase64Encoded": False,
                  "statusCode": 403,
                  "headers": {},
                  "body": "Error"}

    @handler.add(MessageEvent, message=TextMessage)
    def message(line_event):
        text = line_event.message.text #送られてきたメッセージ
        if text=="LEDをつけて":
            line_bot_api.reply_message(line_event.reply_token, TextSendMessage(text="LEDをつけます"))
            payload = {"a":"a"}
            iot.publish(
            topic=topic,
            qos=0,
            payload=json.dumps(payload, ensure_ascii=False)
             )
        elif text=="LEDを消して":
            line_bot_api.reply_message(line_event.reply_token, TextSendMessage(text="LEDを消します"))
            payload = {"b":"b"}
            iot.publish(
            topic=topic,
            qos=0,
            payload=json.dumps(payload, ensure_ascii=False)
             )
        elif text=="電気をつけて":
            line_bot_api.reply_message(line_event.reply_token, TextSendMessage(text="電気をつけます"))
            payload = {"c":"c"}
            iot.publish(
            topic=topic,
            qos=0,
            payload=json.dumps(payload, ensure_ascii=False)
             )
        elif text=="電気を消して":
            line_bot_api.reply_message(line_event.reply_token, TextSendMessage(text="電気を消します"))
            payload = {"d":"d"}
            iot.publish(
            topic=topic,
            qos=0,
            payload=json.dumps(payload, ensure_ascii=False)
             )
        elif text=="エアコンをつけて":
            line_bot_api.reply_message(line_event.reply_token, TextSendMessage(text="エアコンをつけます"))
            payload = {"e":"e"}
            iot.publish(
            topic=topic,
            qos=0,
            payload=json.dumps(payload, ensure_ascii=False)
             )
        elif text=="エアコンを消して":
            line_bot_api.reply_message(line_event.reply_token, TextSendMessage(text="エアコンを消します"))
            payload = {"f":"f"}
            iot.publish(
            topic=topic,
            qos=0,
            payload=json.dumps(payload, ensure_ascii=False)
             )
        else:
            line_bot_api.reply_message(line_event.reply_token, TextSendMessage(text="次の4つのコマンドを受け付けています\n電気をつけて\n電気を消して\nエアコンをつけて\nエアコンを消して"))
    

    try:
        handler.handle(body, signature)
        
        # iot.publish(
        #     topic=topic,
        #     qos=0,
        #     payload=json.dumps(payload, ensure_ascii=False)
        # )
 
        return "Succeeeded."
    except LineBotApiError as e:
        logger.error("Got exception from LINE Messaging API: %s\n" % e.message)
        for m in e.error.details:
            logger.error("  %s: %s" % (m.property, m.message))
        return error_json
    except InvalidSignatureError:
        return error_json

    return ok_json
