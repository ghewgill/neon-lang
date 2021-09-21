namespace csnex
{
    public enum Mode
    {
        read    = 0,
        write   = 1,
    }

    public enum SeekBase
    {
        absolute = 0,
        relative = 1,
        fromEnd  = 2,
    }

    public enum OpenResult
    {
        file  = 0,
        error = 1,
    }

    public enum ReadLineResult
    {
        line  = 0,
        eof   = 1,
        error = 2,
    }
}
