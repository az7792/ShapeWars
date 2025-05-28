# 一个简单http服务器
from flask import Flask, send_from_directory
import os

app = Flask(__name__)

# 设置静态文件夹
app.static_folder = "ShapeWars"
app.static_url_path = "/ShapeWars"

# 处理根路径，返回 index.html
@app.route("/")
def serve_index():
        return send_from_directory(os.path.abspath(os.path.dirname(__file__)), "index.html")

# 处理 ./ShapeWars/... 的请求
@app.route("/ShapeWars/<path:filename>")
def serve_shapewars(filename):
    file_path = os.path.join(os.path.dirname(__file__), "ShapeWars")
    
    # 如果请求的是 .wasm 文件，设置正确的 MIME 类型
    if filename.endswith(".wasm"):
        return send_from_directory(file_path, filename, mimetype="application/wasm")
    
    # 否则直接返回文件
    return send_from_directory(file_path,filename)

# 处理 entityDef 中的 JSON 文件请求
@app.route("/entityDefs/<path:filename>")
def serve_entity_def(filename):
    file_path = os.path.join(os.path.dirname(__file__), "entityDefs")
    return send_from_directory(file_path, filename, mimetype="application/json")

if __name__ == "__main__":
    app.run(debug=True, port=7777, host="0.0.0.0")
