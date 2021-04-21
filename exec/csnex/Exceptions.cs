using System;
using System.Runtime.Serialization;
using System.Security.Permissions;

namespace csnex
{
    [Serializable()]
    public class NeonException: ApplicationException
    {
        public NeonException() {
        }

        public NeonException(string name, string info) : base(name) {
            Name = name;
            Info = info;
        }

        public NeonException(string message) : base(message) {
        }

        public NeonException(string message, params object[] args) : base(string.Format(message, args)) {
        }

        public NeonException(string message, System.Exception innerException) : base(message, innerException) {
        }

        // Satisfy Warning CA2240 to implement a GetObjectData() to our custom exception type.
        [SecurityPermission(SecurityAction.LinkDemand, Flags = SecurityPermissionFlag.SerializationFormatter)]
        public override void GetObjectData(SerializationInfo info, StreamingContext context)
        {
            if (info == null) {
                throw new ArgumentNullException("info");
            }
            info.AddValue("NeonException", Name);
            info.AddValue("NeonInfo", Info);
            base.GetObjectData(info, context);
        }

        public string Name;
        public string Info;
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

    [Serializable]
    public class NeonRuntimeException: NeonException
    {
        public NeonRuntimeException()
        {
        }

        public NeonRuntimeException(string name, string info) : base(name, info)
        {
        }
    }
}
