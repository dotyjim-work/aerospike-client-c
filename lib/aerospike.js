// *****************************************************************************
// Copyright 2013-2016 Aerospike, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License")
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// *****************************************************************************

const as = require('../build/Release/aerospike.node')

const Client = require('./client.js')
const dataTypes = require('./data_types.js')
const filter = require('./filter.js')
const operator = require('./operator.js')

const client = function client (config) {
  return new Client(config)
}

const connect = function connect (config, callback) {
  if (typeof config === 'function') {
    callback = config
    config = {}
  }
  var client = new Client(config)
  client.connect(callback)
  return client
}

const key = function key (ns, set, key) {
  return new dataTypes.Key(ns, set, key)
}

function Aerospike () {
  this.Client = Client
  this.AerospikeError = dataTypes.AerospikeError
  this.Double = dataTypes.Double
  this.GeoJSON = dataTypes.GeoJSON
  this.Key = dataTypes.Key
  this.client = client
  this.connect = connect
  this.filter = filter
  this.indexType = as.indexType
  this.key = key
  this.language = as.language
  this.log = as.log
  this.operator = operator
  this.policy = as.policy
  this.predicates = as.predicates
  this.scanPriority = as.scanPriority
  this.scanStatus = as.scanStatus
  this.status = as.status
}
module.exports = new Aerospike()