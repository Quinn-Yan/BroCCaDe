# This file was automatically generated by bifcl from /home/hendra/CCD/Source/Feature_IAT/src/iat.bif (plugin mode).

export {
module IAT;




global ExtractFeature: function(UID: string , id: conn_id , duration: interval ): bool ;


global feature_event: event(UID: string , id: conn_id , feature: double );

} # end of export section
module GLOBAL;