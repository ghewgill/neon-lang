using System;

namespace csnex
{
    [Serializable()]
    public class NeonException: ApplicationException
    {
        public NeonException() {
        }

        public NeonException(string message) : base(message) {
        }

        public NeonException(string message, params object[] args) : base(string.Format(message, args)) {
        }

        public NeonException(string message, System.Exception innerException) : base(message, innerException) {
        }
    }

    [Serializable]
    public class InvalidOpcodeException: NeonException
    {
        public InvalidOpcodeException() {
        }

        public InvalidOpcodeException(string message) : base(message) {
        }
    }

    [Serializable]
    public class BytecodeException: NeonException 
    {
        public BytecodeException() {
        }

        public BytecodeException(string message) : base(message) {
        }
    }

    [Serializable]
    public class NotImplementedException: NeonException
    {
        public NotImplementedException() {
        }

        public NotImplementedException(string message) : base(message) {
        }
    }
}
