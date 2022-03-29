const http = require("http")

let liveConsumption = 0 //power

//Creates the server that receives the requests.
const server = http.createServer()

function unWrapRequestData(data) {
  data = data.split("/")
  return parseFloat(data[1].split(":")[1])
}

server.on('request', (request, res) => {
  if (request.url === "/"){
    if (request.method === "POST"){
      console.log("POST REQ")
      
      let data = ""

      //Geting the data.
      request.on('data', (chunk) => {
        data = chunk.toString()
      })

      //STUFF BEGINS
      request.on('end', () => {
        liveConsumption = unWrapRequestData(data)

        console.log(data)
        
        res.write(JSON.stringify({result: "success"}))
        res.end()
      })
    } else if (request.method === "GET"){
      console.log("GET REQ")
      res.write(JSON.stringify(liveConsumption))
      res.end()
    }
  }
})

server.listen(80)
