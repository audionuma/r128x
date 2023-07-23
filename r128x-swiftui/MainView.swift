// MainView.swift
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

import SwiftUI
import UniformTypeIdentifiers

struct MainView: View {
  private static let allowedSuffixes: [String] = ["mp3", "mp2", "m4a", "wav", "aif", "aiff", "caf", "alac", "sd2", "ac3", "flac"]
  private static let allowedUTTypes: [UTType] = Self.allowedSuffixes.compactMap { .init(filenameExtension: $0) }

  private static let comdlg32: NSOpenPanel = {
    let result = NSOpenPanel()
    result.allowsMultipleSelection = true
    result.canChooseDirectories = false
    result.prompt = "Process"
    result.title = "File Selector"
    result.message = "Select files…"
    return result
  }()

  // MARK: - Instance.

  public init() {
    Self.comdlg32.allowedContentTypes = Self.allowedUTTypes
  }

  @State private var dragOver = false
  @State private var highlighted: IntelEntry.ID?
  @State private var entries: [IntelEntry] = []

  var progressValue: CGFloat {
    if entries.isEmpty { return 0 }
    return CGFloat(entries.filter(\.processed).count) / CGFloat(entries.count)
  }

  var queueMessage: String {
    if entries.isEmpty {
      return "Drag audio files from Finder to the table in this window."
    }
    let filesPendingProcessing: Int = entries.filter(\.processed.negative).count
    let invalidResults: Int = entries.reduce(0) { $0 + ($1.status == .failed ? 1 : 0) }
    guard filesPendingProcessing == 0 else {
      return "Processing files in the queue: \(filesPendingProcessing) remaining."
    }
    guard invalidResults != 0 else {
      return "All files are processed, excepting \(invalidResults) failed files."
    }
    return "All files are processed, excepting those who have met possible errors."
  }

  var body: some View {
    VStack(spacing: 5) {
      Table(entries, selection: $highlighted) {
        TableColumn("Status", value: \.statusDisplayed).width(50)
        TableColumn("File Name", value: \.fileName)
        TableColumn("Program Loudness", value: \.programLoudnessDisplayed).width(110)
        TableColumn("Loudness Range", value: \.loudnessRangeDisplayed).width(110)
        TableColumn("dBTP", value: \.dBTPDisplayed).width(40)
      }.onChange(of: entries) { _ in
        batchProcess(forced: false)
      }
      .onDrop(of: [UTType.fileURL], isTargeted: $dragOver) { providers -> Bool in
        var counter = 0
        defer {
          if counter > 0 {
            batchProcess(forced: false)
          }
        }
        providers.forEach { provider in
          _ = provider.loadObject(ofClass: URL.self) { url, _ in
            guard let url else { return }
            let path = url.path
            guard !entries.map(\.fileName).contains(path) else { return }
            entryInsertion: for fileExtension in Self.allowedSuffixes {
              guard path.hasSuffix(".\(fileExtension)") else { continue }
              counter += 1
              entries.append(.init(fileName: path))
              break entryInsertion
            }
          }
        }
        return true
      }
      HStack {
        Button("Add Files") {
          guard Self.comdlg32.runModal() == .OK else { return }
          let entriesAsPaths: [String] = entries.map(\.fileName)
          let contents: [URL] = Self.comdlg32.urls.filter {
            !entriesAsPaths.contains($0.path)
          }
          entries.append(contentsOf: contents.map { .init(fileName: $0.path) })
        }
        Button("Clear Table") { entries.removeAll() }
        Button("Reprocess All") { batchProcess(forced: true) }
        ProgressView(value: progressValue) { Text(queueMessage).controlSize(.small) }
        Spacer().frame(minWidth: 50)
      }.padding(.bottom, 10).padding([.horizontal], 10)
    }.frame(minWidth: 971, minHeight: 367, alignment: .center)
  }

  func batchProcess(forced: Bool = false) {
    DispatchQueue.global().async {
      for i in 0 ..< entries.count {
        entries[i].process(forced: forced)
      }
    }
  }
}

struct ContentView_Previews: PreviewProvider {
  static var previews: some View {
    MainView()
  }
}

private extension Bool {
  var negative: Bool { !self }
}
