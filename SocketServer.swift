//
//  SocketServer.swift
//  conecta4
//
//  Created by Marcelo Preciado Fausto on 13/04/24.
//
import Foundation
import Combine
import SwiftUI

protocol SocketServerDelegate {
    func messageReady(message: String )
    func connectionChange(status : ServerStatus)
}

enum ServerStatus : String{
    case DISCONNECTED   = "DISCONNECTED"
    case CONNECTING     = "CONNECTING"
    case CONNECTED      = "CONNECTED"
}

final class SocketServer : ObservableObject {
    
    private     var webSocket       : URLSessionWebSocketTask?
    static      let shared          : SocketServer          = SocketServer()
    private     var timer           : Timer?
    @Published  var connected       : Bool                  = false
    @Published  var status          : ServerStatus          = .DISCONNECTED
                var delegate        : SocketServerDelegate?
                var hostPort        :  Int                  = 0
                var hostIpAddress   : String                = ""
    
    init() {
      
    }
    
    
    func initServer(hostIpAddress: String , hostPort: Int){
        if self.connected {
            if timer?.isValid != nil {
                timer?.invalidate()
            }
            disconnect()
        }
        self.hostIpAddress  = hostIpAddress
        self.hostPort       = hostPort
        timer?.invalidate()
        timer =  Timer.scheduledTimer(timeInterval: 3, target: self, selector: #selector(connect), userInfo: nil, repeats: true)
    }
    
    
    @objc
    func connect() {
        if !connected {
            status      = .CONNECTING
            let url     = URL(string: "ws://\(self.hostIpAddress):\(self.hostPort)")!
            webSocket   = URLSession.shared.webSocketTask(with: url)
            webSocket?.receive(completionHandler: onReceive)
            webSocket?.resume()
        }

    }
    
    func disconnect() {
        if timer?.isValid != nil {
            timer?.invalidate()
        }
        webSocket?.cancel(with: .normalClosure, reason: nil)
        DispatchQueue.main.async {
            if self.connected {
                self.status = .DISCONNECTED
                self.delegate?.connectionChange(status: self.status)
            }
            self.connected = false
        }
        webSocket = nil
    }
    
    private func onReceive(incoming: Result<URLSessionWebSocketTask.Message, Error>) {
        webSocket?.receive(completionHandler: onReceive)
        if case .success(let message) = incoming {
            onMessage(message: message)
        }
        else if case .failure(_) = incoming {
            disconnect()
            DispatchQueue.main.async {
                self.connected = false
            }
        }
    }
    
    private func onMessage(message: URLSessionWebSocketTask.Message) {
        switch message {
            
        case let .data(messageData):
            print("[DATA] \(messageData)")
            break
        case let .string(messageString):
            self.delegate?.messageReady(message: messageString)
            
            DispatchQueue.main.async {
                self.status     = .CONNECTED
                if !self.connected {
                    self.delegate?.connectionChange(status: self.status)
                }
                self.connected  = true
            }
            break

        @unknown default:
            print("[ERROR]")
        }
    }
    
    func send(jsonString: String) {
        webSocket?.send(.string(jsonString)) { error in
            if let error = error {
                print("Error sending message", error)
                self.disconnect()
                self.connect()
            }
        }
    }
    
    deinit {
        connected = false
    }
    
}



