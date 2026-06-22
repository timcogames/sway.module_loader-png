#!/usr/bin/env python3
# encoding: utf-8

from http.server import HTTPServer, SimpleHTTPRequestHandler

class RequestHandler(SimpleHTTPRequestHandler):
  def end_headers(self):
    # CORS
    self.send_header('Access-Control-Allow-Origin', '*')
    self.send_header('Access-Control-Allow-Methods', 'GET')
    self.send_header('Cache-Control', 'no-store, no-cache, must-revalidate')
    # COOP
    self.send_header('Cross-Origin-Opener-Policy', 'same-origin')
    # COEP
    self.send_header('Cross-Origin-Embedder-Policy', 'require-corp')
    return super(RequestHandler, self).end_headers()

httpd = HTTPServer(('localhost', 7800), RequestHandler)
httpd.serve_forever()
