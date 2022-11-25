// tests go here; this will not be compiled when this package is used as an extension.
// Service UUID: 0xF005(16) = 61445(10)
s3linkudk.startService()
s3linkudk.onDisplayTextReceived(function (text) {
    basic.showString(text)
})
