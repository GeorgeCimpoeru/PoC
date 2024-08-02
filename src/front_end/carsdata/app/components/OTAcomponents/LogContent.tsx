import React, { createContext, useState, useContext, FC } from 'react';

// Enum pentru nivelurile de logare
export enum LogLevel {
  INFO = "INFO",
  WARNING = "WARNING",
  ERROR = "ERROR"
}

// Interfață pentru o intrare de log
interface LogEntry {
  message: string;
  level: LogLevel;
  timestamp: Date;
}

// Contextul pentru logare
interface LogContextType {
  logs: LogEntry[];
  addLog: (message: string, level: LogLevel) => void;
}

const LogContext = createContext<LogContextType | undefined>(undefined);

// Tipurile de proprietăți pentru LogProvider
interface LogProviderProps {
  children: React.ReactNode;
}

const LogProvider: FC<LogProviderProps> = ({ children }) => {
  const [logs, setLogs] = useState<LogEntry[]>([]);

  const addLog = (message: string, level: LogLevel) => {
    const newLog: LogEntry = { message, level, timestamp: new Date() };
    setLogs((prevLogs) => [...prevLogs, newLog]);
  };

  return (
    <LogContext.Provider value={{ logs, addLog }}>
      {children}
    </LogContext.Provider>
  );
};

// Tipurile de proprietăți pentru LogModal
interface LogModalProps {
  isOpen: boolean;
  onClose: () => void;
}

const LogModal: React.FC<LogModalProps> = ({ isOpen, onClose }) => {
  const logContext = useContext(LogContext);

  if (!logContext) {
    return null;
  }

  return (
    <div className={`modal ${isOpen ? 'open' : 'closed'}`}>
      <div className="modal-content">
        <span className="close-button" onClick={onClose}>×</span>
        <h2>Informații de Logare</h2>
        <div>
          {logContext.logs.map((log, index) => (
            <div key={index} style={{ color: getColor(log.level) }}>
              <strong>{log.level}</strong> [{log.timestamp.toLocaleString()}]: {log.message}
            </div>
          ))}
        </div>
      </div>
      <style jsx>{`
        .modal {
          display: flex;
          justify-content: center;
          align-items: center;
          position: fixed;
          top: 0;
          left: 0;
          width: 100%;
          height: 100%;
          background-color: rgba(0, 0, 0, 0.5);
          transition: opacity 0.3s ease;
        }
        .modal.closed {
          opacity: 0;
          visibility: hidden;
        }
        .modal.open {
          opacity: 1;
          visibility: visible;
        }
        .modal-content {
          background: white;
          padding: 20px;
          border-radius: 5px;
          position: relative;
        }
        .close-button {
          position: absolute;
          top: 10px;
          right: 10px;
          cursor: pointer;
        }
      `}</style>
    </div>
  );
};

// Funcția pentru a obține culoarea în funcție de nivel
const getColor = (level: LogLevel): string => {
  switch (level) {
    case LogLevel.INFO:
      return 'blue';
    case LogLevel.WARNING:
      return 'orange';
    case LogLevel.ERROR:
      return 'red';
    default:
      return 'black';
  }
};

// Componenta pentru logare
const Logger: React.FC = () => {
  const logContext = useContext(LogContext);

  if (!logContext) {
    return null;
  }

  const logInfo = () => {
    logContext.addLog("Aceasta este un mesaj informativ", LogLevel.INFO);
  };

  const logWarning = () => {
    logContext.addLog("Aceasta este un mesaj de avertizare", LogLevel.WARNING);
  };

  const logError = () => {
    logContext.addLog("Aceasta este un mesaj de eroare", LogLevel.ERROR);
  };

  return (
    <div>
      <button onClick={logInfo}>Log Info</button>
      <button onClick={logWarning}>Log Warning</button>
      <button onClick={logError}>Log Error</button>
    </div>
  );
};

// Componenta principală
const App: React.FC = () => {
  const [isModalOpen, setModalOpen] = useState(false);

  const openModal = () => setModalOpen(true);
  const closeModal = () => setModalOpen(false);

  return (
    <LogProvider>
      <div className="App">
        <h1>Pagina de Logare</h1>
        <button onClick={openModal}>Afișează Logurile</button>
        <Logger />
        <LogModal isOpen={isModalOpen} onClose={closeModal} />
      </div>
    </LogProvider>
  );
};

export default App;
