// IntelEntry.swift
// This file is part of r128x.
//
// r128x is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// r128x is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with r128x.  If not, see <http://www.gnu.org/licenses/>.
// copyright Manuel Naudin 2012-2013

import Foundation

public enum StatusForProcessing: String {
  case processing = "…"
  case succeeded = "✔︎"
  case failed = "✖︎"
}

public struct IntelEntry: Identifiable, Equatable {
  public let id = UUID()
  public let fileName: String
  public var programLoudness: Double?
  public var loudnessRange: Double?
  public var dBTP: Double?
  public var status: StatusForProcessing = .processing
  var processed: Bool = false

  public init(fileName: String) {
    self.fileName = fileName
  }

  public var statusDisplayed: String { status.rawValue }
  public var programLoudnessDisplayed: String { programLoudness?.description ?? "N/A" }
  public var loudnessRangeDisplayed: String {
    guard let loudnessRange = loudnessRange else { return "N/A" }
    let format = "%.\(2)f"
    return String(format: format, loudnessRange)
  }

  public var dBTPDisplayed: String {
    guard let dBTP = dBTP else { return "N/A" }
    let format = "%.\(2)f"
    return String(format: format, dBTP)
  }

  public mutating func process(forced: Bool = false) {
    if processed, !forced { return }
    status = .processing
    var osStatus: OSStatus = noErr
    var il: Double = .infinity * -1
    var lra: Double = .infinity * -1
    var max_tp: Float32 = .infinity * -1
    osStatus = ExtAudioReader(fileName as CFString, &il, &lra, &max_tp)
    processed = true
    guard osStatus == noErr else {
      status = .failed
      return
    }
    programLoudness = il
    loudnessRange = lra
    dBTP = Double(max_tp)
    status = .succeeded
  }
}
