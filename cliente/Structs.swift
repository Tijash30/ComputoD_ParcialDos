//
//  Structs.swift
//  conecta4
//
//  Created by Marcelo Preciado Fausto on 13/04/24.
//

import Foundation
import SwiftUI


struct PlayerMessage: Decodable, Encodable{
    var inst : String = ""
    var id : Int = 0
    var fila : Int = 0
    var columna : Int = 0
}

struct PlayerInfo: Decodable, Encodable{
    var inst : String = ""
    var user : String = ""
}

struct IDinfo: Decodable, Encodable{
    var inst : String = ""
    var id : Int = 0
}

class Player: ObservableObject{
    static var shared : Player = Player(user: "", email: "", password: "")
    @Published var update : Bool = false
    @Published var errorMessage : String = ""
    @Published var loginCorrect : Bool = false
    @Published var ID : Int = 0
    @Published var win : Int = 0
    
    init(user: String, email: String, password: String) {
        self.user = user
        self.email = email
        self.password = password
    }
    var user : String = ""
    var email : String = ""
    var password : String = ""
}

struct CircleStruct{
    var color: Color
    let position: Int
}



class Tablero: ObservableObject{
    static var shared : Tablero = Tablero()
    @Published var update : Bool = false
    @Published var tablero: [[CircleStruct]] = {
        var initialTablero: [[CircleStruct]] = []
        for _ in 0..<7 {
            var row: [CircleStruct] = []
            for column in 0..<7 {
                row.append(CircleStruct(color: .gray, position: column))
            }
            initialTablero.append(row)
        }
        return initialTablero
    }()

    func agregarFicha(id: Int, fila: Int, columna: Int){        
   
        if id == 1{
            tablero[fila][columna].color = .yellow
        }else if id == 2{
            tablero[fila][columna].color = .red
        }
    }
}
