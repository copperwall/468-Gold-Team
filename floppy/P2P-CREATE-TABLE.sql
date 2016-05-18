-- 21 bytes of varchar
-- 3 bytes of padding
-- 32 bytes of ints
-- No padding
-- Record size 56
CREATE TABLE NodeStats (
   IPAddress  VARCHAR(20),
   BlocksUploaded  INT,
   BlocksDownloaded INT,
   RequestsReceived INT,
   RequestsSent INT,
   RequestsServed INT,
   RequestsFailed INT,
   DistrosUploaded  INT,
   DistrosDownloaded INT,
   PRIMARY KEY (IPAddress),
 );

-- 21 bytes of varchar
-- 3 bytes of padding
-- 4 bytes of datetime(int)
-- 4 bytes of datetime(int)
-- No padding
-- Record size 32
CREATE TABLE Nodes  VOLATILE(
   IPAddress  VARCHAR(20),
   firstSeen    DATETIME,
   lastSeen    DATETIME,
   PRIMARY KEY (IPAddress),
   FOREIGN KEY (IPAddress) REFERENCES NodeStats
);

-- 4 bytes of int
-- 65 bytes of varchar
-- 3 bytes of padding
-- 4 bytes of int
-- 4 bytes of int
-- No padding
-- Record size 80 bytes
CREATE TABLE Distros (
    DistroID  INT,    -- may change to VARCHAR if desired
    DistroName  VARCHAR(64),
    NFiles  INT,
    Size   INT,
    PRIMARY KEY(DistroID)
)

-- 4 bytes of int
-- 4 bytes of int
-- 129 bytes of varchar
-- 3 bytes of padding
-- 4 bytes of int
-- 4 bytes of int
-- Record size 148 bytes
CREATE TABLE Files (
   FileId      INT,
   DistroId  INT,
   FileName  VARCHAR(128),
   Size  INT,
   Blocks INT,
   PRIMARY KEY(FileId),
   FOREIGN KEY(DistroId) REFERENCES (Distros)
);









-- Record size 40
CREATE TABLE Availability VOLATILE (
    NodeId  VARCHAR(20),
    DistroId  INT,
    FileId  INT,
    BlockID  INT,
    TimeStamp DATETIME,
    PRIMARY KEY(FileId, BlockId, NodeId),
    FOREIGN KEY(NodeId) REFERENCES Nodes,
    FOREIGN KEY(NodeId) REFERNCES NodeStats,
   FOREIGN KEY (DistroId) REFERENCES Distros,
   FOREIGN KEY (FileId) REFERENCES Files
);


-- Record size 16
CREATE TABLE LocalAvailability  (
   DistroID  INT,
  FileId INT,
  BlockId INT,
  TimeStamp DATETIME,
   PRIMARY KEY(FileId, BlockId),
   FOREIGN KEY (DistroId) REFERENCES Distros,
   FOREIGN KEY (FileId) REFERENCES Files
);



-- 4 bytes of int
-- 4 bytes of datetime(int)
-- 11 bytes of varchar
-- 3 bytes of padding
-- 4 bytes of datetime(int)
-- 4 bytes of int
CREATE TABLE Downloads(
   DistroID INT,
   StartTime DATETIME,
   Status VARCHAR(10),    -- ENUM, but we do not have ENUMs
   EndTime DATETIME,
   Success INT
);























-- 21 bytes of varchar
-- 3 bytes of padding
-- 4 bytes of int
-- 4 bytes of int
-- 4 bytes of int
-- 4 bytes of datetime(int)
-- 11 bytes of varchar
-- 3 bytes of padding
-- 4 bytes of datetime(int)
-- Record size 58
CREATE TABLE OutgoingRequests VOLATILE (
  NodeID VARCHAR(20),
  DistroID INT,
  FileID INT,
  BlockID INT,
  TimeStamp DATETIME,
  Status VARCHAR(10),
  ArchivalTime DATETIME,
  PRIMARY KEY(FileID, BlockId, NodeID, TimeStamp),
  FOREIGN KEY(NodeID) REFERENCES Nodes,
  FOREIGN KEY(NodeID) REFERENCES NodeStats,
  FOREIGN KEY(DistroID) REFERENCES Distros,
  FOREIGN KEY(FileID) REFERENCES Files
);


-- 21 bytes of varchar
-- 3 bytes of padding
-- 4 bytes of int
-- 4 bytes of int
-- 4 bytes of int
-- 4 bytes of datetime(int)
-- 11 bytes of varchar
-- 3 bytes of padding
-- 4 bytes of datetime(int)
-- Record size 58
CREATE TABLE IncomingRequests VOLATILE (
  NodeID VARCHAR(20),
  DistroID INT,
  FileID INT,
  BlockID INT,
  TimeStamp DATETIME,
  Status VARCHAR(10),
  ArchivalTime DATETIME,
  PRIMARY KEY(FileID, BlockId, NodeID, TimeStamp),
  FOREIGN KEY(NodeID) REFERENCES Nodes,
  FOREIGN KEY(NodeID) REFERENCES NodeStats,
  FOREIGN KEY(DistroID) REFERENCES Distros,
  FOREIGN KEY(FileID) REFERENCES Files
);
