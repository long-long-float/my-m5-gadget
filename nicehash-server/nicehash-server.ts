import * as http from 'http'
import got from 'got'

const nicehashHost = 'http://[::1]:18000'

const server = http.createServer(async (req: http.IncomingMessage, res: http.ServerResponse) => {
  const workers: any = await got.get(`${nicehashHost}/workers`).json()
  res.end(workers.workers[0].algorithms[0].speed.toString())
})

server.listen('18080')
