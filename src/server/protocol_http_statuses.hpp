// Author: Dmitry Kukovinets (d1021976@gmail.com)

// Usage: HTTP_STATUS(continue_, 100, "Continue");
// Effect: you have variable server::http::status::continue_
// of type server::http::status (static member of class),
// with code = 100 and description = std::string("Continue").

// WARNING: do NOT include this file in any files except protocol_http.h
// (inside class status) or protocol_http.cpp (aften including protocol_http.h)!
// Include protocol_http.h instead!

// WARNING: don't forget insert
//	#define SERVER_PROTOCOL_HTTP_CPP
// in protocol_http.cpp before including this header!

#ifdef SERVER_PROTOCOL_HTTP_CLASS_STATUS
// Declaration in header
#	define HTTP_STATUS(status_name_, code_, description_)			\
		static const status status_name_
#else
// Definition in .cpp
#	define HTTP_STATUS(status_name_, code_, description_)			\
		const server::http::status server::http::status:: status_name_(code_, description_)
#endif	// defined SERVER_PROTOCOL_HTTP_CPP


// 1xx: Informational
HTTP_STATUS(continue_,							100,	"Continue"							);
HTTP_STATUS(switching_protocols,				101,	"Switching Protocols"				);
HTTP_STATUS(processing,							102,	"Processing"						);
HTTP_STATUS(name_not_resolved,					105,	"Name Not Resolved"					);

// 2xx: Success
HTTP_STATUS(ok,									200,	"OK"								);
HTTP_STATUS(created,							201,	"Created"							);
HTTP_STATUS(accepted,							202,	"Accepted"							);
HTTP_STATUS(non_authoritative_information,		204,	"Non-Authoritative Information"		);
HTTP_STATUS(no_content,							204,	"No Content"						);
HTTP_STATUS(reset_content,						205,	"Reset Content"						);
HTTP_STATUS(partial_content,					206,	"Partial Content"					);
HTTP_STATUS(multi_status,						207,	"Multi-Status"						);
HTTP_STATUS(im_used,							226,	"IM Used"							);

// 3xx: Redirection
HTTP_STATUS(multiple_choices,					300,	"Multiple Choices"					);
HTTP_STATUS(moded_permanently,					301,	"Moved Permanently"					);
HTTP_STATUS(moved_temporarily,					302,	"Moved Temporarily"					);
HTTP_STATUS(found,								302,	"Found"								);
HTTP_STATUS(see_other,							303,	"See Other"							);
HTTP_STATUS(not_modified,						304,	"Not Modified"						);
HTTP_STATUS(use_proxy,							305,	"Use Proxy"							);
HTTP_STATUS(temporary_redirect,					307,	"Temporary Redirect"				);

// 4xx: Client Error
HTTP_STATUS(bad_request,						400,	"Bad Request"						);
HTTP_STATUS(unauthorized,						401,	"Unauthorized"						);
HTTP_STATUS(payment_required,					402,	"Payment Required"					);
HTTP_STATUS(forbidden,							403,	"Forbidden"							);
HTTP_STATUS(not_found,							404,	"Not Found"							);
HTTP_STATUS(method_not_allowed,					405,	"Method Not Allowed"				);
HTTP_STATUS(not_acceptable,						406,	"Not Acceptable"					);
HTTP_STATUS(proxy_authentication_required,		407,	"Proxy Authentication Required"		);
HTTP_STATUS(request_timeout,					408,	"Request Timeout"					);
HTTP_STATUS(conflict,							409,	"Conflict"							);
HTTP_STATUS(gone,								410,	"Gone"								);
HTTP_STATUS(length_required,					411,	"Length Required"					);
HTTP_STATUS(precondition_failed,				412,	"Precondition Failed"				);
HTTP_STATUS(request_entity_too_large,			413,	"Request Entity Too Large"			);
HTTP_STATUS(request_uri_too_large,				414,	"Request-URI Too Large"				);
HTTP_STATUS(unsupported_media_type,				415,	"Unsupported Media Type"			);
HTTP_STATUS(requested_range_not_satisfiable,	416,	"Requested Range Not Satisfiable"	);
HTTP_STATUS(expectation_failed,					417,	"Expectation Failed"				);
HTTP_STATUS(im_a_teapot,						418,	"I'm a teapot"						);
HTTP_STATUS(unprocessable_entity,				422,	"Unprocessable Entity"				);
HTTP_STATUS(locked,								423,	"Locked"							);
HTTP_STATUS(failed_dependency,					424,	"Failed Dependency"					);
HTTP_STATUS(unordered_collection,				425,	"Unordered Collection"				);
HTTP_STATUS(upgrade_required,					426,	"Upgrade Required"					);
HTTP_STATUS(precondition_required,				428,	"Precondition Required"				);
HTTP_STATUS(too_many_requests,					429,	"Too Many Requests"					);
HTTP_STATUS(request_header_fields_too_large,	431,	"Request Header Fields Too Large"	);
HTTP_STATUS(requested_host_unavailable,			434,	"Requested Host Unavailable"		);
HTTP_STATUS(retry_with,							449,	"Retry With"						);
HTTP_STATUS(unavailable_for_legal_reasons,		451,	"Unavailable For Legal Reasons"		);
HTTP_STATUS(unrecoverable_error,				456,	"Unrecoverable Error"				);

// 5xx: Server Error
HTTP_STATUS(internal_server_error,				500,	"Internal Server Error"				);
HTTP_STATUS(not_implemented,					501,	"Not Implemented"					);
HTTP_STATUS(bad_gateway,						502,	"Bad Gateway"						);
HTTP_STATUS(service_unavailable,				503,	"Service Unavailable"				);
HTTP_STATUS(gateway_timeout,					504,	"Gateway Timeout"					);
HTTP_STATUS(http_version_not_supported,			505,	"HTTP Version Not Supported"		);
HTTP_STATUS(variant_also_negotiates,			506,	"Variant Also Negotiates"			);
HTTP_STATUS(insufficient_storage,				507,	"Insufficient Storage"				);
HTTP_STATUS(loop_detected,						508,	"Loop Detected"						);
HTTP_STATUS(bandwidth_limit_exceeded,			509,	"Bandwidth Limit Exceeded"			);
HTTP_STATUS(not_extended,						510,	"Not Extended"						);
HTTP_STATUS(network_authentication_required,	511,	"Network Authentication Required"	);


#undef HTTP_STATUS
