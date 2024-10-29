class Logger {
    private logs: string[];

    constructor() {
        /* Retrieve stored logs from local storage. If none exists initialize empty array */
        const storedLogs = localStorage.getItem('logs');
        this.logs = storedLogs ? JSON.parse(storedLogs) : [];

        /* Intercept console methods to capture logs
           Logging will be done using the same syntax for console methods */
        const originalLog = console.log.bind(console);
        const originalWarn = console.warn.bind(console);
        const originalError = console.error.bind(console);

        /* Override console methods for logs, warnings and errors */
        console.log = (...args: any[]) => {
            this.captureLog('LOG', args);
            originalLog(...args);
        };

        console.warn = (...args: any[]) => {
            this.captureLog('WARNING', args);
            originalWarn(...args);
        };

        console.error = (...args: any[]) => {
            this.captureLog('ERROR', args);
            originalError(...args);
        };
    }

    /* Formats a date into the desired format [DD-MM-YYYY HH:mm:ss.milliseconds] */
    private formatDate(date: Date): string {
        const day = String(date.getDate()).padStart(2, '0');
        const month = String(date.getMonth() + 1).padStart(2, '0');
        const year = date.getFullYear();
        const hours = String(date.getHours()).padStart(2, '0');
        const minutes = String(date.getMinutes()).padStart(2, '0');
        const seconds = String(date.getSeconds()).padStart(2, '0');
        const milliseconds = String(date.getMilliseconds()).padStart(3, '0');

        return `[${day}-${month}-${year} ${hours}:${minutes}:${seconds}.${milliseconds}]`;
    }

    /* Captures a log message with the specified level (log, warn, error) and arguments */
    private captureLog(level: string, args: any[]) : void {
        const formattedDate = this.formatDate(new Date());

        /* Pretty print JSON arguments with 2 spaces for indentation */
        const logMsgParts = args.map(arg => {
            if (typeof arg === 'object') {
                    return JSON.stringify(arg, null, 2);
            }
            return String(arg);
        });

        const log_msg = `${formattedDate} [${level}]: ${logMsgParts.join(' ')}`;
        this.logs.push(log_msg);

        /* Store the updated logs array into the local storage */
        localStorage.setItem('logs', JSON.stringify(this.logs));
    }

    /* Dummy method that does nothing. Used at the beginning of the file to make sure the import is not ignored */
    public init(): void {
        return;
    }

    /* When is called, the logs are downloaded as a text file */
    public downloadLogs(): void {

        const storedLogs = localStorage.getItem('logs');
        const logsToDownload = storedLogs ? JSON.parse(storedLogs) : [];

        /* Check if there are logs to download */
        if (logsToDownload.length === 0) {
            console.log('No logs available');
            return;
        }

        /* Joins the log into a single string with line breaks; 
           Creates a blod and an URL for a downloadable text file */
        const logContent = logsToDownload.join('\n');
        const blob = new Blob([logContent], { type: 'text/plain'});
        const url = URL.createObjectURL(blob);

        /* Create a temporary anchor element to trigger the download */
        const a = document.createElement('a');
        a.href = url;
        a.download = 'logs.txt';
        document.body.appendChild(a);
        a.click();
        document.body.removeChild(a);
        URL.revokeObjectURL(url);

        /* After the download, clears the logs array and the local storage */
        this.logs = [];
        localStorage.removeItem('logs');
    }
}

const logger = new Logger();

export default logger;