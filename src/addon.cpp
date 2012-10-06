#include <node.h>
#include <node_buffer.h>
#include <v8.h>
#include <cstddef>
#include <cassert>
#include <cstring>

extern "C" {
	#include "KeccakNISTInterface.h"
}

#define MAX_DIGEST_SIZE 64
#define ASSERT_IS_STRING_OR_BUFFER(val) \
	if (!val->IsString() && !Buffer::HasInstance(val)) { \
		return ThrowException(Exception::TypeError(String::New("Not a string or buffer"))); \
	}

using namespace node;
using namespace v8;

static void toHex(const char *data_buf, size_t size, char *output);

class SHA3Hash: public ObjectWrap {
public:
	hashState state;
	int32_t bitlen;

	static void
	Initialize(Handle<Object> target) {
		HandleScope scope;
		Local<FunctionTemplate> t = FunctionTemplate::New(New);
		t->InstanceTemplate()->SetInternalFieldCount(1);
		t->SetClassName(String::NewSymbol("SHA3Hash"));

		NODE_SET_PROTOTYPE_METHOD(t, "update", Update);
		NODE_SET_PROTOTYPE_METHOD(t, "digest", Digest);
		NODE_SET_PROTOTYPE_METHOD(t, "hexdigest", Hexdigest);

		target->Set(String::NewSymbol("SHA3Hash"), t->GetFunction());
	}

	static Handle<Value>
	New(const Arguments &args) {
		HandleScope scope;
		SHA3Hash *obj;
		int32_t hashlen;

		hashlen = args[0]->IsUndefined() ? 512 : args[0]->Int32Value();
		obj = new SHA3Hash();
		obj->bitlen = hashlen;
		::Init(&obj->state, hashlen);
		obj->Wrap(args.This());

		return args.This();
	}

	static Handle<Value>
	Update(const Arguments &args) {
		HandleScope scope;
		SHA3Hash *obj = ObjectWrap::Unwrap<SHA3Hash>(args.This());

		ASSERT_IS_STRING_OR_BUFFER(args[0]);
		enum encoding enc = ParseEncoding(args[1]);
		ssize_t len = DecodeBytes(args[0], enc);
		
		if (len < 0) {
			Local<Value> exception = Exception::TypeError(String::New("Bad argument"));
			return ThrowException(exception);
		}

		if (Buffer::HasInstance(args[0])) {
			Local<Object> buffer_obj = args[0]->ToObject();
			const char *buffer_data = Buffer::Data(buffer_obj);
			size_t buffer_length = Buffer::Length(buffer_obj);
			::Update(&obj->state, (const BitSequence *) buffer_data, buffer_length * 8);
		} else {
			char *buf = new char[len];
			ssize_t written = DecodeWrite(buf, len, args[0], enc);
			assert(written == len);
			::Update(&obj->state, (const BitSequence *) buf, len * 8);
			delete[] buf;
		}

		return Undefined();
	}

	static Handle<Value>
	Digest(const Arguments &args) {
		HandleScope scope;
		SHA3Hash *obj = ObjectWrap::Unwrap<SHA3Hash>(args.This());
		hashState state2;
		unsigned char digest[MAX_DIGEST_SIZE];

		memcpy(&state2, &obj->state, sizeof(hashState));
		Final(&state2, digest);
		return String::New((const char *) digest, obj->bitlen / 8);
	}

	static Handle<Value>
	Hexdigest(const Arguments &args) {
		HandleScope scope;
		SHA3Hash *obj = ObjectWrap::Unwrap<SHA3Hash>(args.This());
		hashState state2;
		unsigned char digest[MAX_DIGEST_SIZE];
		char hexdigest[MAX_DIGEST_SIZE * 2];

		memcpy(&state2, &obj->state, sizeof(hashState));
		Final(&state2, digest);
		toHex((const char *) digest, obj->bitlen / 8, hexdigest);

		return String::New((const char *) hexdigest, obj->bitlen / 4);
	}
};

static const char hex_chars[] = {
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
	'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j',
	'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't',
	'u', 'v', 'w', 'x', 'y', 'z'
};

static void
toHex(const char *data_buf, size_t size, char *output) {
	size_t i;
	
	for (i = 0; i < size; i++) {
		output[i * 2] = hex_chars[(unsigned char) data_buf[i] / 16];
		output[i * 2 + 1] = hex_chars[(unsigned char) data_buf[i] % 16];
	}
}

static void
init(Handle<Object> target) {
	SHA3Hash::Initialize(target);
}

NODE_MODULE(sha3, init)
