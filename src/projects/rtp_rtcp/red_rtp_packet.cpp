#include "red_rtp_packet.h"

RedRtpPacket::RedRtpPacket()
{
}

RedRtpPacket::RedRtpPacket(RedRtpPacket &src)
	:RtpPacket(src)
{
	_block_pt = src._block_pt;
}

RedRtpPacket::~RedRtpPacket()
{

}

void RedRtpPacket::PackageAsRed(uint8_t red_payload_type, RtpPacket &src)
{
	SetMarker(src.Marker());
	SetPayloadType(src.PayloadType());
	SetUlpfec(src.IsUlpfec(), src.OriginPayloadType());
	SetSsrc(src.Ssrc());
	SetSequenceNumber(src.SequenceNumber());
	SetTimestamp(src.Timestamp());

	_payload_offset = src.HeadersSize();
	_payload_size = src.PayloadSize();
	_padding_size = src.PaddingSize();
	_extension_size = src.ExtensionSize();

	PackageAsRed(red_payload_type);

	SetPayload(src.Payload(), src.PayloadSize());
}

//Implement RED as part of the RTP header to reduce memory copying and improve performance.
void RedRtpPacket::PackageAsRed(uint8_t red_payload_type)
{
	// TODO(Getroot): Check validation
	// This function should be used after SetCsrcs, Extensions and before AllocatePayload.

	_block_pt = PayloadType();
	SetPayloadType(red_payload_type);

	// Increase 1 bytes for RED
	_payload_offset = _payload_offset + RED_HEADER_SIZE;
	_data->SetLength(_payload_offset);
	_buffer = _data->GetWritableDataAs<uint8_t>();

	// Write payload type at the end of the rtp header
	_buffer[_payload_offset - RED_HEADER_SIZE] = _block_pt;
}

uint8_t RedRtpPacket::BlockPT()
{
	return _block_pt;
}