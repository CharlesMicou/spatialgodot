/* Copyright (c) Improbable Worlds Ltd, All Rights Reserved. */
#ifndef IMPROBABLE_PROTOBUF_INTEROP_INCLUDE_IMPROBABLE_PBIO_H
#define IMPROBABLE_PROTOBUF_INTEROP_INCLUDE_IMPROBABLE_PBIO_H
/* See README.md for an overview of the protobuf-interop library. */
#if defined(PBIO_DLL) && (defined(_MSC_VER) || defined(__ORBIS__))
#ifdef PBIO_DLL_EXPORT
#define PBIO_API __declspec(dllexport)
#else /* PBIO_DLL_EXPORT */
#define PBIO_API __declspec(dllimport)
#endif /* PBIO_DLL_EXPORT */
#else /* defined(PBIO_DLL) && (defined(_MSC_VER) || defined(__ORBIS__)) */
#define PBIO_API
#endif /* defined(PBIO_DLL) && (defined(_MSC_VER) || defined(__ORBIS__)) */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <stdint.h>
#include <stddef.h>

typedef uint32_t Pbio_FieldId;

struct Pbio;
struct Pbio_Object;
typedef struct Pbio Pbio;
typedef struct Pbio_Object Pbio_Object;
typedef void Pbio_AllocatorState;
typedef void* Pbio_Allocate (size_t, Pbio_AllocatorState*);
typedef void Pbio_Deallocate (void*, size_t, Pbio_AllocatorState*);

/** Creates a protobuf serializer. Must be destroyed with Pbio_Destroy. */
PBIO_API Pbio* Pbio_Create();
/**
 * Creates a protobuf serializer with custom allocate and deallocate functions. allocator_state
 * is passed to both each time they are called. If allocate throws or returns nullptr, the
 * behaviour is undefined. There are no alignment requirements for allocate.
 * Must be destroyed with Pbio_Destroy.
 */
PBIO_API Pbio* Pbio_Create2(Pbio_Allocate* allocate,
                            Pbio_Deallocate* deallocate,
                            Pbio_AllocatorState* allocator_state);
/** Destroys a protobuf serializer, invalidating all Pbio_Objects obtained from it. */
PBIO_API void Pbio_Destroy(Pbio* pbio);
/** Returns nonzero iff any object associated with the given Pbio encountered an error. */
PBIO_API uint8_t Pbio_HasError(const Pbio* pbio);
/**
 * Obtains the length of the most recent error encountered by any object associated with the given
 * Pbio.
 */
PBIO_API uint32_t Pbio_GetErrorLength(const Pbio* pbio);
/**
 * Obtains the most recent error encountered by any object associated with the given Pbio.
 * The provided buffer must have space at least equal to the length returned by
 * Pbio_GetErrorLength.
 */
PBIO_API void Pbio_GetError(const Pbio* pbio, uint8_t* buffer);
/** Obtains the root object of a protobuf serializer. */
PBIO_API Pbio_Object* Pbio_GetRootObject(Pbio* pbio);

/** Completely clears all fields in the given object. */
PBIO_API void Pbio_Clear(Pbio_Object* object);
/** Completely clears the given field ID in the given object. */
PBIO_API void Pbio_ClearField(Pbio_Object* object, Pbio_FieldId field_id);
/**
 * Copies all fields from `src` to `dst`. The copy is shallow; changes made to object fields in the
 * source will also be reflected in the copied fields.
 *
 * If `src == dst`, or if the objects are not associated with the same root Pbio instance, no
 * operation is performed.
 */
PBIO_API void Pbio_ShallowCopy(const Pbio_Object* src, Pbio_Object* dst);
/**
 * Copies over a field from `src` to `dst`. If multiple fields with the given field_id exist all
 * are copied. The copy is shallow; changes made to object fields in the source will also be
 * reflected in the copied fields.
 *
 * If `src == dst`, or if the objects are not associated with the same root Pbio instance, no
 * operation is performed.
 */
PBIO_API void Pbio_ShallowCopyField(const Pbio_Object* src, Pbio_Object* dst,
                                    Pbio_FieldId field_id);

/**
 * Allocates an orphaned Pbio_Object in memory owned by the given Pbio_Object instance. The returned
 * object is owned by the associated Pbio instance, but is not reachable from any other object. The
 * memory is freed by a call to Pbio_Destroy.
 */
PBIO_API Pbio_Object* Pbio_AllocateObject(const Pbio_Object* pbio);
/**
 * Allocates a buffer of the specified length in bytes from memory owned by the given Pbio_Object
 * instance. The memory is freed by a call to Pbio_Destroy.
 */
PBIO_API uint8_t* Pbio_AllocateBuffer(Pbio_Object* object, uint32_t length);
/**
 * Merges the given buffer into the given object, appending all fields. This function
 * can fail; if the return value is zero, call Pbio_GetError to obtain an error string.
 */
PBIO_API uint8_t Pbio_MergeFromBuffer(Pbio_Object* object, const uint8_t* buffer, uint32_t length);
/** Computes the serialized length of the given Pbio_Object. */
PBIO_API uint32_t Pbio_GetWriteBufferLength(const Pbio_Object* object);
/**
 * Serializes the given object into the provided buffer, which _must_ have space at
 * least equal to the length returned by Pbio_WriteBufferLength. This function can
 * fail; if the return value is zero, call Pbio_GetError to obtain an error string.
 */
PBIO_API uint8_t Pbio_WriteToBuffer(const Pbio_Object* object, uint8_t* buffer);

/** Returns the number of unique field IDs used in the Pbio_Object. */
PBIO_API uint32_t Pbio_GetUniqueFieldIdCount(const Pbio_Object* object);
/**
 * Returns the sorted list of unique field IDs used in the Pbio_Object. The buffer parameter
 * must have space remaining for as many field IDs as indicated by Pbio_GetUniqueFieldIdCount.
 */
PBIO_API void Pbio_GetUniqueFieldIds(const Pbio_Object* object, uint32_t* buffer);

/* Functions that append a single value to the set of fields present in an object. For any field ID,
 * these can be called repeatedly to construct a list of values, or mixed freely with the AddList
 * functions below; however, making a single call to an AddList function is the most efficient way
 * to construct a list of values. Note that, for best performance, fields should be added to the
 * object in field ID order. */
PBIO_API void Pbio_AddFloat(Pbio_Object* object, Pbio_FieldId field_id, float value);
PBIO_API void Pbio_AddDouble(Pbio_Object* object, Pbio_FieldId field_id, double value);
PBIO_API void Pbio_AddBool(Pbio_Object* object, Pbio_FieldId field_id, uint8_t value);
PBIO_API void Pbio_AddInt32(Pbio_Object* object, Pbio_FieldId field_id, int32_t value);
PBIO_API void Pbio_AddInt64(Pbio_Object* object, Pbio_FieldId field_id, int64_t value);
PBIO_API void Pbio_AddUint32(Pbio_Object* object, Pbio_FieldId field_id, uint32_t value);
PBIO_API void Pbio_AddUint64(Pbio_Object* object, Pbio_FieldId field_id, uint64_t value);
PBIO_API void Pbio_AddSint32(Pbio_Object* object, Pbio_FieldId field_id, int32_t value);
PBIO_API void Pbio_AddSint64(Pbio_Object* object, Pbio_FieldId field_id, int64_t value);
PBIO_API void Pbio_AddFixed32(Pbio_Object* object, Pbio_FieldId field_id, uint32_t value);
PBIO_API void Pbio_AddFixed64(Pbio_Object* object, Pbio_FieldId field_id, uint64_t value);
PBIO_API void Pbio_AddSfixed32(Pbio_Object* object, Pbio_FieldId field_id, int32_t value);
PBIO_API void Pbio_AddSfixed64(Pbio_Object* object, Pbio_FieldId field_id, int64_t value);
PBIO_API void Pbio_AddBytes(Pbio_Object* object, Pbio_FieldId field_id, const uint8_t* buffer,
                            uint32_t length);
PBIO_API Pbio_Object* Pbio_AddObject(Pbio_Object* object, Pbio_FieldId field_id);

/* Functions that append a list of primitive values for a particular field ID to an object. Note
 * that, for best performance, fields should be added to the object in field ID order. */
PBIO_API void Pbio_AddFloatList(Pbio_Object* object, Pbio_FieldId field_id, const float* values,
                                uint32_t count);
PBIO_API void Pbio_AddDoubleList(Pbio_Object* object, Pbio_FieldId field_id, const double* values,
                                 uint32_t count);
PBIO_API void Pbio_AddBoolList(Pbio_Object* object, Pbio_FieldId field_id, const uint8_t* values,
                               uint32_t count);
PBIO_API void Pbio_AddInt32List(Pbio_Object* object, Pbio_FieldId field_id, const int32_t* values,
                                uint32_t count);
PBIO_API void Pbio_AddInt64List(Pbio_Object* object, Pbio_FieldId field_id, const int64_t* values,
                                uint32_t count);
PBIO_API void Pbio_AddUint32List(Pbio_Object* object, Pbio_FieldId field_id, const uint32_t* values,
                                 uint32_t count);
PBIO_API void Pbio_AddUint64List(Pbio_Object* object, Pbio_FieldId field_id, const uint64_t* values,
                                 uint32_t count);
PBIO_API void Pbio_AddSint32List(Pbio_Object* object, Pbio_FieldId field_id, const int32_t* values,
                                 uint32_t count);
PBIO_API void Pbio_AddSint64List(Pbio_Object* object, Pbio_FieldId field_id, const int64_t* values,
                                 uint32_t count);
PBIO_API void Pbio_AddFixed32List(Pbio_Object* object, Pbio_FieldId field_id,
                                  const uint32_t* values, uint32_t count);
PBIO_API void Pbio_AddFixed64List(Pbio_Object* object, Pbio_FieldId field_id,
                                  const uint64_t* values, uint32_t count);
PBIO_API void Pbio_AddSfixed32List(Pbio_Object* object, Pbio_FieldId field_id,
                                   const int32_t* values, uint32_t count);
PBIO_API void Pbio_AddSfixed64List(Pbio_Object* object, Pbio_FieldId field_id,
                                   const int64_t* values, uint32_t count);

/* Functions that determine the number of occurrences of a particular field ID in an object.
 *
 * Note that, for best performance, fields should be accessed in field ID order. */
PBIO_API uint32_t Pbio_GetFloatCount(const Pbio_Object* object, Pbio_FieldId field_id);
PBIO_API uint32_t Pbio_GetDoubleCount(const Pbio_Object* object, Pbio_FieldId field_id);
PBIO_API uint32_t Pbio_GetBoolCount(const Pbio_Object* object, Pbio_FieldId field_id);
PBIO_API uint32_t Pbio_GetInt32Count(const Pbio_Object* object, Pbio_FieldId field_id);
PBIO_API uint32_t Pbio_GetInt64Count(const Pbio_Object* object, Pbio_FieldId field_id);
PBIO_API uint32_t Pbio_GetUint32Count(const Pbio_Object* object, Pbio_FieldId field_id);
PBIO_API uint32_t Pbio_GetUint64Count(const Pbio_Object* object, Pbio_FieldId field_id);
PBIO_API uint32_t Pbio_GetSint32Count(const Pbio_Object* object, Pbio_FieldId field_id);
PBIO_API uint32_t Pbio_GetSint64Count(const Pbio_Object* object, Pbio_FieldId field_id);
PBIO_API uint32_t Pbio_GetFixed32Count(const Pbio_Object* object, Pbio_FieldId field_id);
PBIO_API uint32_t Pbio_GetFixed64Count(const Pbio_Object* object, Pbio_FieldId field_id);
PBIO_API uint32_t Pbio_GetSfixed32Count(const Pbio_Object* object, Pbio_FieldId field_id);
PBIO_API uint32_t Pbio_GetSfixed64Count(const Pbio_Object* object, Pbio_FieldId field_id);
PBIO_API uint32_t Pbio_GetBytesCount(const Pbio_Object* object, Pbio_FieldId field_id);
PBIO_API uint32_t Pbio_GetObjectCount(const Pbio_Object* object, Pbio_FieldId field_id);

/* Functions that access a single value for a particular field ID in an object. These
 * functions assume the field is non-repeated, i.e. if the field appears multiple times in the
 * object, it is are interpreted as a single field according to the protobuf spec.
 *
 * If the field does not exist, a default value is returned; call the corresponding GetCount
 * function above to determine if the field is present.
 *
 * Note that, for best performance, fields should be accessed in field ID order. */
PBIO_API float Pbio_GetFloat(const Pbio_Object* object, Pbio_FieldId field_id);
PBIO_API double Pbio_GetDouble(const Pbio_Object* object, Pbio_FieldId field_id);
PBIO_API uint8_t Pbio_GetBool(const Pbio_Object* object, Pbio_FieldId field_id);
PBIO_API int32_t Pbio_GetInt32(const Pbio_Object* object, Pbio_FieldId field_id);
PBIO_API int64_t Pbio_GetInt64(const Pbio_Object* object, Pbio_FieldId field_id);
PBIO_API uint32_t Pbio_GetUint32(const Pbio_Object* object, Pbio_FieldId field_id);
PBIO_API uint64_t Pbio_GetUint64(const Pbio_Object* object, Pbio_FieldId field_id);
PBIO_API int32_t Pbio_GetSint32(const Pbio_Object* object, Pbio_FieldId field_id);
PBIO_API int64_t Pbio_GetSint64(const Pbio_Object* object, Pbio_FieldId field_id);
PBIO_API uint32_t Pbio_GetFixed32(const Pbio_Object* object, Pbio_FieldId field_id);
PBIO_API uint64_t Pbio_GetFixed64(const Pbio_Object* object, Pbio_FieldId field_id);
PBIO_API int32_t Pbio_GetSfixed32(const Pbio_Object* object, Pbio_FieldId field_id);
PBIO_API int64_t Pbio_GetSfixed64(const Pbio_Object* object, Pbio_FieldId field_id);
PBIO_API uint32_t Pbio_GetBytesLength(const Pbio_Object* object, Pbio_FieldId field_id);
PBIO_API const uint8_t* Pbio_GetBytes(const Pbio_Object* object, Pbio_FieldId field_id);
PBIO_API Pbio_Object* Pbio_GetObject(Pbio_Object* object, Pbio_FieldId field_id);

/* Functions that access a value by index for a particular field ID in an object.
 *
 * If the index doesn't exist for the given field, a default is returned; call
 * the corresponding GetCount function above to to determine if the total number of fields.
 *
 * Note that, for best performance, fields should be accessed in field ID and index order. */
PBIO_API float Pbio_IndexFloat(const Pbio_Object* object, Pbio_FieldId field_id, uint32_t index);
PBIO_API double Pbio_IndexDouble(const Pbio_Object* object, Pbio_FieldId field_id, uint32_t index);
PBIO_API uint8_t Pbio_IndexBool(const Pbio_Object* object, Pbio_FieldId field_id, uint32_t index);
PBIO_API int32_t Pbio_IndexInt32(const Pbio_Object* object, Pbio_FieldId field_id, uint32_t index);
PBIO_API int64_t Pbio_IndexInt64(const Pbio_Object* object, Pbio_FieldId field_id, uint32_t index);
PBIO_API uint32_t Pbio_IndexUint32(const Pbio_Object* object, Pbio_FieldId field_id,
                                   uint32_t index);
PBIO_API uint64_t Pbio_IndexUint64(const Pbio_Object* object, Pbio_FieldId field_id,
                                   uint32_t index);
PBIO_API int32_t Pbio_IndexSint32(const Pbio_Object* object, Pbio_FieldId field_id, uint32_t index);
PBIO_API int64_t Pbio_IndexSint64(const Pbio_Object* object, Pbio_FieldId field_id, uint32_t index);
PBIO_API uint32_t Pbio_IndexFixed32(const Pbio_Object* object, Pbio_FieldId field_id,
                                    uint32_t index);
PBIO_API uint64_t Pbio_IndexFixed64(const Pbio_Object* object, Pbio_FieldId field_id,
                                    uint32_t index);
PBIO_API int32_t Pbio_IndexSfixed32(const Pbio_Object* object, Pbio_FieldId field_id,
                                    uint32_t index);
PBIO_API int64_t Pbio_IndexSfixed64(const Pbio_Object* object, Pbio_FieldId field_id,
                                    uint32_t index);
PBIO_API uint32_t Pbio_IndexBytesLength(const Pbio_Object* object, Pbio_FieldId field_id,
                                        uint32_t index);
PBIO_API const uint8_t* Pbio_IndexBytes(const Pbio_Object* object, Pbio_FieldId field_id,
                                        uint32_t index);
PBIO_API Pbio_Object* Pbio_IndexObject(Pbio_Object* object, Pbio_FieldId field_id, uint32_t index);

/* Functions that copy the complete list of values for a particular field ID in an object. The
 * provided output array must have space for at least as many elements as returned by the
 * corresponding GetCount function.
 *
 * Note that, for best performance, fields should be accessed in field ID and index order. */
PBIO_API void Pbio_GetFloatList(const Pbio_Object* object, Pbio_FieldId field_id,
                                float* output_array);
PBIO_API void Pbio_GetDoubleList(const Pbio_Object* object, Pbio_FieldId field_id,
                                 double* output_array);
PBIO_API void Pbio_GetBoolList(const Pbio_Object* object, Pbio_FieldId field_id,
                               uint8_t* output_array);
PBIO_API void Pbio_GetInt32List(const Pbio_Object* object, Pbio_FieldId field_id,
                                int32_t* output_array);
PBIO_API void Pbio_GetInt64List(const Pbio_Object* object, Pbio_FieldId field_id,
                                int64_t* output_array);
PBIO_API void Pbio_GetUint32List(const Pbio_Object* object, Pbio_FieldId field_id,
                                 uint32_t* output_array);
PBIO_API void Pbio_GetUint64List(const Pbio_Object* object, Pbio_FieldId field_id,
                                 uint64_t* output_array);
PBIO_API void Pbio_GetSint32List(const Pbio_Object* object, Pbio_FieldId field_id,
                                 int32_t* output_array);
PBIO_API void Pbio_GetSint64List(const Pbio_Object* object, Pbio_FieldId field_id,
                                 int64_t* output_array);
PBIO_API void Pbio_GetFixed32List(const Pbio_Object* object, Pbio_FieldId field_id,
                                  uint32_t* output_array);
PBIO_API void Pbio_GetFixed64List(const Pbio_Object* object, Pbio_FieldId field_id,
                                  uint64_t* output_array);
PBIO_API void Pbio_GetSfixed32List(const Pbio_Object* object, Pbio_FieldId field_id,
                                   int32_t* output_array);
PBIO_API void Pbio_GetSfixed64List(const Pbio_Object* object, Pbio_FieldId field_id,
                                   int64_t* output_array);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* IMPROBABLE_PROTOBUF_INTEROP_INCLUDE_IMPROBABLE_PBIO_H */
