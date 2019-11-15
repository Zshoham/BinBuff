using System;
using System.Collections.Generic;
using System.Text;

namespace BinBuff
{
    public interface ISerializable
    {
        void Serialize(BinBuffer binBuffer);

        void Deserialize(BinBuffer binBuffer);
    }
}
