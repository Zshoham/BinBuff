using System;
using System.Collections.Generic;
using System.Text;

namespace CSSerializer
{
	public interface ISerializable
	{
		void Serialize(Buffer buffer);

		void Deserialize(Buffer buffer);
	}
}
