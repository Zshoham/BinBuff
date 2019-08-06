/**
 * The interface implements serialization and deserialization into and from a Buffer.
 * Classes that implement this interface will always use the provided method when written
 * into a Buffer.
 * @see Buffer
 */
public interface ISerializable {

    void serialize(Buffer buffer);

    void deserialize(Buffer buffer);
}
