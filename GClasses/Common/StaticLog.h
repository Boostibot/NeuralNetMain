#ifndef STATICLOG_H
#define STATICLOG_H

#include "G_Common.h"
#include "Libraries/Fmt/fmt/core.h"

/*
template <typename Derived, typename DerivedDataPackage>
class CrtpBase : public DerivedDataPackage
{
        //The following functions are obligatory to be overloaded
    public:
        template <typename Type>
        inline void OverloadedFunc(Type arg)
        {
            static_cast<Derived POINTER>(this)->InterpretArg(arg);
        }

};

//The default logger interpret
struct CrtpDerivedPackage
{
    public:
        int MyInt1;
        int MyInt2;
        int MyInt3;
};
class CrtpDerived : public CrtpBase<CrtpDerived, CrtpDerivedPackage>
{
    public:
        using ThisPackageType = CrtpDerivedPackage;

        //The obligatory function overloads
    public:
        template <typename Type>
        inline void OverloadedFunc(Type arg)
        {
            this->MyInt1 = 1; //Avalible to use
        }
};
*/

struct LogWriterTraits
{
        template<typename LoggerT>
        static constexpr bool IsLogWriter()
        {
            return std::is_base_of<LogWriterTraits, LoggerT>();
        }
        template<typename TemplateSpecialisationType, template<typename ... ClassTypes> class LogWriterTemplate>
        static constexpr bool IsLogWriter()
        {
            return std::is_base_of<LogWriterTraits, LogWriterTemplate<TemplateSpecialisationType>>();
        }
};

struct LogTraits
{
        template<typename LoggerT>
        static constexpr bool IsLogger()
        {
            return std::is_base_of<LogTraits, LoggerT>();
        }
        template<typename TemplateSpecialisationType, template<typename ... ClassTypes> class LoggerTemplate>
        static constexpr bool IsLogger()
        {
            return std::is_base_of<LogTraits, LoggerTemplate<TemplateSpecialisationType>>();
        }
};

template<typename SameTo, typename ... ArgumentTypes>
struct AreTypesSame
{
        template<bool indetifier, typename FirstType, typename ... OtherArgumentTypes>
        constexpr static bool RecursiveCall()
        {
            if(std::is_same<FirstType, SameTo>::value)
                return true;
            else
                return RecursiveCall<indetifier, OtherArgumentTypes...>();
        }
        template<bool indetifier>
        constexpr static bool RecursiveCall()
        {
            return false;
        }

        static constexpr bool value = RecursiveCall<true, ArgumentTypes...>();

};

//Put into a file
//#include "Libraries/Fmt/fmt/chrono.h"
//#include "Libraries/Fmt/fmt/color.h"
#include "Libraries/Fmt/fmt/compile.h"
#include "Libraries/Fmt/fmt/core.h"
//#include "Libraries/Fmt/fmt/format-inl.h"
//#include "Libraries/Fmt/fmt/format.h"
//#include "Libraries/Fmt/fmt/locale.h"
#include "Libraries/Fmt/fmt/os.h"
//#include "Libraries/Fmt/fmt/ostream.h"
//#include "Libraries/Fmt/fmt/printf.h"
//#include "Libraries/Fmt/fmt/ranges.h"

#include <array>
namespace StaticLogger
{
    using StringType = ::std::string;
    using StringViewType = ::std::string_view;


    template <typename Derived,
              typename DerivedDataPackage = typename Derived::ThisPackageType>
    class Logger : public DerivedDataPackage, public LogWriterTraits
    {
        public:
            using StringType = ::std::string;
            using StringViewType = ::std::string_view;
            using ThisType = Logger<Derived, DerivedDataPackage>;

        public:
            static constexpr u32 LevelCount = 16; //This should be equal to the max number of levels suported
            static constexpr u32 MaxLevelIndex = LevelCount - 1;
            //static constexpr u32 InitialStringBufferSize = 256;
            //static constexpr u32 InitialTempStringSize = 128;

        private:
            std::array<bool, LevelCount> LevelLoggingStatus;
            bool LoggingStatus = true;

        public:
            Derived REF GetWriter() {return POINTER_VALUE(static_cast<Derived POINTER>(this));}

        public:
            Logger() : DerivedDataPackage()
            {
                this->SetUp();
            }
            Logger(const ThisType PASS_REF other) : DerivedDataPackage(other)
            {
                this->LevelLoggingStatus = other.LevelLoggingStatus;
                this->LoggingStatus = other.LoggingStatus;
                this->GetWriter().OnConstruction();
            }
            Logger(ThisType PASS_RVALUE_REF other) : DerivedDataPackage(std::move(other))
            {
                //Move constructor just copies instead of swapping because its faster
                this->LevelLoggingStatus = other.LevelLoggingStatus;
                this->LoggingStatus = other.LoggingStatus;
                this->GetWriter().OnConstruction();
            }

            //Any other constructor for the DerivedDataPackage
            template<typename ... ArgumentTypes ,
                     //Checks assure that the ArgumentTypes are not identical to the ones of copy and move constructor
                     //This is to resolve the ambiguous function call
                     //Checks if any of the types arent same as the contructor types but only blocks the function if the type is alone (sizeof...(ArgumentTypes) == 1)
                     std::enable_if_t<!(AreTypesSame<ThisType PASS_REF, ArgumentTypes...>::value && (sizeof...(ArgumentTypes) == 1)), int> = 0
                     >
            Logger(ArgumentTypes PASS_RVALUE_REF ... args) : DerivedDataPackage(std::forward<ArgumentTypes>(args)...)
            {
                //TypeViewer(std::forward<ArgumentTypes>(args)...);
                this->SetUp();
            }
            ~Logger()
            {
                GetWriter().OnDestruction();
            }

            /*
            template<typename FirstType, typename... ArgTypes>
            void TypeViewer(FirstType PASS_RVALUE_REF first, ArgTypes PASS_RVALUE_REF... args)
            {
                (void)first;
                std::string name = typeid (FirstType).name();
                (void)name;
                TypeViewer(std::forward<ArgTypes>(args)...);
            }
            void TypeViewer()
            {

            }
            */
        public:
            ThisType REF operator=(ThisType PASS_RVALUE_REF) = default;
            ThisType REF operator=(const ThisType PASS_REF) = default;


            //Overloaded functions - begin
        public:
            template<u32 level, typename ... MsgTypes>
            void WriteMsgs(MsgTypes ... msgs)
            {
                if(this->IsLogEnabled<level>())
                    GetWriter().WriteMsgs(msgs...);
            }
            template<u32 level, typename ... NameTypes, typename ... VarTypes>
            void WriteVars(NameTypes ... names, VarTypes ... vars)
            {
                if(this->IsLogEnabled<level>())
                    GetWriter().template WriteVars<NameTypes..., VarTypes...>(names..., vars...);
            }

            template<u32 level, typename ... AdditionalTypes>
            void AppendSource(StringViewType file, const u32 lineNum, AdditionalTypes ... additional)
            {
                if(this->IsLogEnabled<level>())
                    GetWriter().AppendSource(file, lineNum, additional...);
            }
            template<u32 level, typename ... TagTypes>
            void AppendTags(TagTypes ... tags)
            {
                if(this->IsLogEnabled<level>())
                    GetWriter().AppendTags(tags...);
            }

            template<typename ... PathTypes>
            void OpenStream(PathTypes ... paths)
            {
                GetWriter().OpenStream(paths...);
            }
            template<typename ... StreamIdentifierTypes>
            void CloseStream(StreamIdentifierTypes ... indetifiers)
            {
                GetWriter().CloseStream(indetifiers...);
            }
            template<typename ... StreamIdentifierTypes>
            bool IsStreamOpen(StreamIdentifierTypes ... indetifiers)
            {
                return GetWriter().IsStreamOpen(indetifiers...);
            }
            template<typename ... ArgTypes>
            void FlushStream(ArgTypes ... args)
            {
                GetWriter().FlushStream(args...);
            }

        private:
            void OnConstruction()
            {
                GetWriter().OnConstruction();
            }
            void OnDestruction()
            {
                GetWriter().OnDestruction();
            }
            //Overloaded functions - end


        public:
            static constexpr bool IsLevelInRange(const u32 level)
            {
                return (level < ThisType::LevelCount);
            }
            inline void DoLog(const bool onOff)
            {
                LoggingStatus = onOff;
            }
            inline void EnableLog()
            {
                this->DoLog(true);
            }
            inline void DisableLog()
            {
                this->DoLog(false);
            }

            template <u32 ... levels>
            void DoLoggingLevels(const bool onOff)
            {
                //Checks all levels
                constexpr bool dummy = ThisType::UnravelAndCheckLevel_Assert<true, levels...>(); //Forces the function to be evaluated at compile time
                (void)dummy;

                //Sets them
                this->UnravelAndSetLoggingLevel<true, levels...>(onOff);
                //true is identifier used to identify the variadic template - is there another way??
            }
            template <typename ... LevelTypes>
            void DoLoggingLevels(const bool onOff, const LevelTypes ... levels)
            {
                //Checks all levels
                this->UnravelAndCheckLevel_Throw(levels...);

                //Sets them
                this->UnravelAndSetLoggingLevel(onOff, levels...);
                //Both could be done in a single recursive functions but if invalid arguments are
                //  provided no levels should be set before the invalid argiment is encountered
                //  ie. Logger.DoLoggingLevels(true, 0, 1, 2, 123, 4) would still set levels 0, 1, 2 and throw
                //  important to notice the level 4 wouldnt be set - this is why this separate checking is necessary
            }

            inline bool IsLogEnabled() const
            {
                return LoggingStatus;
            }
            inline bool IsLogEnabled(const u32 level) const
            {
                this->ThrowIfLevelIsOutOfRange(level);
                return this->IsLogEnabledInternal(level);
            }
            template<u32 level>
            inline bool IsLogEnabled() const
            {
                constexpr bool dummy = ThisType::AssertIfLevelIsOutOfRange<level>(); //Forces the function to be evaluated at compile time
                (void)dummy;
                return this->IsLogEnabledInternal(level);
            }

        private:
            template<u32 level>
            static constexpr bool AssertIfLevelIsOutOfRange()
            {
                static_assert (ThisType::IsLevelInRange(level), "Logger: The provided level is out of array range;");
                return ThisType::IsLevelInRange(level);
            }
            static void ThrowIfLevelIsOutOfRange(const u32 level)
            {
                if(NOT(ThisType::IsLevelInRange(level)))
                    throw std::runtime_error("Logger: The provided level is out of array range;");
            }

            inline bool IsLogEnabledInternal(const u32 level) const
            {
                if(IsLogEnabled())
                    return LevelLoggingStatus[level];
                else
                    return false;
            }

            inline void DoLoggingLevelInternal(const u32 level, const bool onOff)
            {
                this->LevelLoggingStatus[level] = onOff;
            }

            //Setting
            template<bool idnetifier, u32 firstLevel, u32 ... levels>
            inline void UnravelAndSetLoggingLevel(const bool onOff)
            {
                this->DoLoggingLevelInternal(firstLevel, onOff);
                UnravelAndSetLoggingLevel<idnetifier, levels...>(onOff);
            }
            template<bool idnetifier>
            inline void UnravelAndSetLoggingLevel(const bool) {}

            template<typename FirstLevelType, typename ... LevelTypes>
            inline void UnravelAndSetLoggingLevel(const bool onOff, const FirstLevelType firstLevel, const LevelTypes ... levels)
            {
                this->DoLoggingLevelInternal(firstLevel, onOff);
                UnravelAndSetLoggingLevel(onOff, levels...);
            }
            inline void UnravelAndSetLoggingLevel(const bool) {}

            //Checking
            template<bool idnetifier, u32 firstLevel, u32 ... levels>
            static constexpr bool UnravelAndCheckLevel_Assert()
            {
                ThisType::AssertIfLevelIsOutOfRange<firstLevel>();
                return UnravelAndCheckLevel_Assert<idnetifier, levels...>();
            }
            template<bool idnetifier>
            static constexpr bool UnravelAndCheckLevel_Assert() {return true;}

            template<typename FirstLevelType, typename ... LevelTypes>
            inline void UnravelAndCheckLevel_Throw(const FirstLevelType firstLevel, const LevelTypes ... levels)
            {
                this->ThrowIfLevelIsOutOfRange(firstLevel);
                UnravelAndCheckLevel_Throw(levels...);
            }
            inline void UnravelAndCheckLevel_Throw() {}

            //SetUp
            inline void SetUp()
            {
                LevelLoggingStatus.fill(true);
                GetWriter().OnConstruction();
            }
    };

    namespace StaticLogTesting
    {
        struct TestingDataPackage1
        {
            StringType FileName = "";
            u32 LineNum = 0;
            StringType Tags = "";
            StringType LastFunctionCalled = "";
            StringType Identifier = "TestingDataPackage";
            bool StreamState = false;
            StringType POINTER OnDestructionWriteLocation = nullptr;
        };

        struct TestingLogger1 : public Logger<TestingLogger1, TestingDataPackage1>
        {
            public:
                using ThisPackageType = StaticLogTesting::TestingDataPackage1;
            public:
                using StringType = ::std::string;
                using StringViewType = ::std::string_view;

                template<typename ... MsgTypes>
                void WriteMsgs(MsgTypes ... )                   {this->LastFunctionCalled = "WriteMsgs";}
                template<typename ... NameTypes, typename ... VarTypes>
                void WriteVars(NameTypes ..., VarTypes ...)     {this->LastFunctionCalled = "WriteVars";}

                template<typename ... AdditionalTypes>
                void AppendSource(StringViewType file, const u32 lineNum)
                {
                    this->LastFunctionCalled = "AppendSource";
                    this->FileName = file;
                    this->LineNum = lineNum;
                }
                template<typename ... TagTypes>
                void AppendTags(TagTypes ...)                   {this->LastFunctionCalled = "AppendTags";}

                void OpenStream(StringType)                     {this->LastFunctionCalled = "OpenStream";  this->StreamState = true;}
                void CloseStream()                              {this->LastFunctionCalled = "CloseStream"; this->StreamState = false;}
                bool IsStreamOpen()                             {this->LastFunctionCalled = "IsStreamOpen"; return this->StreamState;}
                void FlushStream()                              {this->LastFunctionCalled = "FlushStream";}

                void OnConstruction()                                       {this->LastFunctionCalled = "OnConstruction";}
                void OnDestruction()
                {
                    this->LastFunctionCalled = "OnDestruction";
                    if(OnDestructionWriteLocation != nullptr)
                        POINTER_VALUE(OnDestructionWriteLocation) = "OnDestruction";
                }
        };

        struct TestingDataPackage2
        {
            public:
            int MyInt1 = 1;
            int MyInt2 = 2;
            int MyInt3 = 3;
            StringType LastFunctionCalled = "";
            StringType Identifier = "TestingDataPackage2";
            bool StreamState = false;
        };

        struct TestingLogger2 : public Logger<TestingLogger2, TestingDataPackage2>
        {
            public:
                using ThisPackageType = StaticLogTesting::TestingDataPackage2;
            public:
                using StringType = ::std::string;
                using StringViewType = ::std::string_view;

                template<typename ... MsgTypes>
                void WriteMsgs(MsgTypes ... )                   {this->LastFunctionCalled = "WriteMsgs";}
                template<typename ... NameTypes, typename ... VarTypes>
                void WriteVars(NameTypes ..., VarTypes ...)     {this->LastFunctionCalled = "WriteVars";}

                template<typename ... AdditionalTypes>
                void AppendSource(StringViewType, const u32)
                {
                    this->LastFunctionCalled = "AppendSource";
                }
                template<typename ... TagTypes>
                void AppendTags(TagTypes ...)                   {this->LastFunctionCalled = "AppendTags";}

                void OpenStream(StringType)                     {this->LastFunctionCalled = "OpenStream";  this->StreamState = true;}
                void CloseStream()                              {this->LastFunctionCalled = "CloseStream"; this->StreamState = false;}
                bool IsStreamOpen()                             {this->LastFunctionCalled = "IsStreamOpen"; return this->StreamState;}
                void FlushStream()                              {this->LastFunctionCalled = "FlushStream";}

                void OnConstruction()                           {this->LastFunctionCalled = "OnConstruction";}
                void OnDestruction()                            {this->LastFunctionCalled = "OnDestruction";}

        };



        struct TestingDataPackageMove
        {
            u32 LineNum = 0;
            StringType Str = "";

            TestingDataPackageMove(StringViewType str) : Str(str) {}
            TestingDataPackageMove(u32 lineNum, StringViewType str) : LineNum(lineNum), Str(str) {}
            TestingDataPackageMove() = delete;
            TestingDataPackageMove(const TestingDataPackageMove PASS_REF) = delete;
            TestingDataPackageMove(TestingDataPackageMove PASS_RVALUE_REF other)
            {
                //Swapping is to make the fact that the move really happened visbible while testing
                std::swap(this->LineNum, other.LineNum);
                this->Str.swap(other.Str);
            };

            //Assignment
            TestingDataPackageMove REF operator=(TestingDataPackageMove PASS_RVALUE_REF other)
            {
                //Swapping is to make the fact that the move really happened visbible while testing
                std::swap(this->LineNum, other.LineNum);
                this->Str.swap(other.Str);
                return POINTER_VALUE(this);
            }
            TestingDataPackageMove REF operator=(const TestingDataPackageMove PASS_REF) = delete ;

        };

        struct TestingLoggerMove : public Logger<TestingLoggerMove, TestingDataPackageMove>
        {
                using ThisPackageType = StaticLogTesting::TestingDataPackageMove;
                using LoggerType = Logger<TestingLoggerMove, TestingDataPackageMove>;

                void WriteMsgs() {}
                void WriteVars() {}
                void AppendSource() {}
                void AppendTags() {}
                void OpenStream() {}
                void CloseStream() {}
                bool IsStreamOpen() {return false;}
                void FlushStream() {}
                void OnConstruction() {}
                void OnDestruction() {}
                TestingLoggerMove() = delete;
                TestingLoggerMove(const TestingLoggerMove PASS_REF) = delete;
                TestingLoggerMove(TestingLoggerMove PASS_RVALUE_REF) = default;

                TestingLoggerMove REF operator=(TestingLoggerMove PASS_RVALUE_REF other) = default;
                TestingLoggerMove REF operator=(const TestingLoggerMove PASS_REF) = delete ;
        };

        struct TestingDataPackageCopy
        {
            u32 LineNum = 0;
            StringType Str = "";

            TestingDataPackageCopy(StringViewType str) : Str(str) {}
            TestingDataPackageCopy(u32 lineNum, StringViewType str) : LineNum(lineNum), Str(str) {}
            TestingDataPackageCopy() = delete;
            TestingDataPackageCopy(const TestingDataPackageCopy PASS_REF) = default;
            TestingDataPackageCopy(TestingDataPackageCopy PASS_RVALUE_REF) = delete;

            //Assignment
            TestingDataPackageCopy REF operator=(TestingDataPackageCopy PASS_RVALUE_REF) = delete;
            TestingDataPackageCopy REF operator=(const TestingDataPackageCopy PASS_REF) = default;

        };

        struct TestingLoggerCopy : public Logger<TestingLoggerCopy, TestingDataPackageCopy>
        {
                using ThisPackageType = StaticLogTesting::TestingDataPackageCopy;
                using LoggerType = Logger<TestingLoggerCopy, TestingDataPackageCopy>;

                void WriteMsgs() {}
                void WriteVars() {}
                void AppendSource() {}
                void AppendTags() {}
                void OpenStream() {}
                void CloseStream() {}
                bool IsStreamOpen() {return false;}
                void FlushStream() {}
                void OnConstruction() {}
                void OnDestruction() {}
                TestingLoggerCopy() = delete;
                TestingLoggerCopy(const TestingLoggerCopy PASS_REF) = default;
                TestingLoggerCopy(TestingLoggerCopy PASS_RVALUE_REF) = delete;

                TestingLoggerCopy REF operator=(TestingLoggerCopy PASS_RVALUE_REF) = delete;
                TestingLoggerCopy REF operator=(const TestingLoggerCopy PASS_REF other) = default;
        };
    }


    class DefaultDataInterpret
    {
        public:
            fmt::format_int Formater;

        public:
            inline void InterpretArg (int val, std::string PASS_REF outputString)                   {Formater.format_signed(val); outputString.assign(Formater.data(), Formater.size());}
            inline void InterpretArg (long val, std::string PASS_REF outputString)                  {Formater.format_signed(val); outputString.assign(Formater.data(), Formater.size());}
            inline void InterpretArg (long long val, std::string PASS_REF outputString)             {Formater.format_signed(val); outputString.assign(Formater.data(), Formater.size());}
            inline void InterpretArg (unsigned val, std::string PASS_REF outputString)              {Formater.format_unsigned(val); outputString.assign(Formater.data(), Formater.size());}
            inline void InterpretArg (unsigned long val, std::string PASS_REF outputString)         {Formater.format_unsigned(val); outputString.assign(Formater.data(), Formater.size());}
            inline void InterpretArg (unsigned long long val, std::string PASS_REF outputString)    {Formater.format_unsigned(val); outputString.assign(Formater.data(), Formater.size());}
            /*
            inline void InterpretArg (int val, std::string PASS_REF outputString)                  {outputString.clear(); fmt::format_to(std::back_inserter(outputString), FMT_STRING("{:-d}"), val);}
            inline void InterpretArg (long val, std::string PASS_REF outputString)                 {outputString.clear(); fmt::format_to(std::back_inserter(outputString), FMT_STRING("{:-d}"), val);}
            inline void InterpretArg (long long val, std::string PASS_REF outputString)            {outputString.clear(); fmt::format_to(std::back_inserter(outputString), FMT_STRING("{:-d}"), val);}
            inline void InterpretArg (unsigned val, std::string PASS_REF outputString)             {outputString.clear(); fmt::format_to(std::back_inserter(outputString), FMT_STRING("{:d}"), val);}
            inline void InterpretArg (unsigned long val, std::string PASS_REF outputString)        {outputString.clear(); fmt::format_to(std::back_inserter(outputString), FMT_STRING("{:d}"), val);}
            inline void InterpretArg (unsigned long long val, std::string PASS_REF outputString)   {outputString.clear(); fmt::format_to(std::back_inserter(outputString), FMT_STRING("{:d}"), val);}
            */
            static inline void InterpretArg (float val, std::string PASS_REF outputString)                {outputString.clear(); fmt::format_to(std::back_inserter(outputString), FMT_STRING("{:f}"), val);}
            static inline void InterpretArg (double val, std::string PASS_REF outputString)               {outputString.clear(); fmt::format_to(std::back_inserter(outputString), FMT_STRING("{:f}"), val);}
            static inline void InterpretArg (long double val, std::string PASS_REF outputString)          {outputString.clear(); fmt::format_to(std::back_inserter(outputString), FMT_STRING("{:.16f}"), val);}
            static inline void InterpretArg (void POINTER val, std::string PASS_REF outputString)         {outputString.clear(); fmt::format_to(std::back_inserter(outputString), FMT_STRING("{:p}"), val);}

            static inline void InterpretArg (bool val, std::string PASS_REF outputString)
            {
                if(val)
                    outputString = "true";
                else
                    outputString = "false";
            }
            static inline void InterpretArg (const char* val, std::string PASS_REF outputString)
            {
                outputString = val;
            }
            static inline void InterpretArg (std::string val, std::string PASS_REF outputString)
            {
                outputString = val;
            }
            static inline void InterpretArg (std::string_view val, std::string PASS_REF outputString)
            {
                outputString = val;
            }
    };

    //The default logger interpret
    struct DefaultLogWriterPackage
    {
        public:
            using DataInterpretType = DefaultDataInterpret;

        public:
            DefaultLogWriterPackage(const StringViewType filePath)
            {
                this->SetUp(filePath);
            }
            DefaultLogWriterPackage(DefaultLogWriterPackage PASS_RVALUE_REF other)
                : Interpret(std::move(other.Interpret)),
                  Formater(std::move(other.Formater)),
                  File(std::move(other.File)),

                  CollectionString(std::move(other.CollectionString)),
                  TempString(std::move(other.TempString)),

                  Messages(std::move(other.Messages)),
                  Variables(std::move(other.Variables)),
                  Tags(std::move(other.Tags)),
                  Source(std::move(other.Source)),

                  EntryIndex(std::move(other.EntryIndex))
            {
                other.OpenFile("TempFile");
            }
            DefaultLogWriterPackage REF operator=(DefaultLogWriterPackage PASS_RVALUE_REF other)
            {
                this->Interpret = (std::move(other.Interpret));
                this->Formater = (std::move(other.Formater));
                //this->File = (std::move(other.File)) //Dont! Must be done this way to
                this->File.swap(other.File);

                this->CollectionString = (std::move(other.CollectionString));
                this->TempString = (std::move(other.TempString));

                this->Messages = (std::move(other.Messages));
                this->Variables = (std::move(other.Variables));
                this->Tags = (std::move(other.Tags));
                this->Source = (std::move(other.Source));

                this->EntryIndex = (std::move(other.EntryIndex));

                return POINTER_VALUE(this);
            }

            DefaultLogWriterPackage(const DefaultLogWriterPackage PASS_REF) = delete;
            void operator=(const DefaultLogWriterPackage PASS_REF) = delete ;

        protected:
            DataInterpretType Interpret;

        protected:
            fmt::format_int Formater;
            //TODO - make into a data structure
            std::unique_ptr<fmt::ostream> File;
            std::string CollectionString;
            std::string TempString;

            std::string Messages;
            std::string Variables;
            std::string Tags;
            std::string Source;

            u32 EntryIndex;

        protected:
            void SetUp(const StringViewType filePath)
            {
                this->OpenFile(filePath);
                this->ReserveStrings();

                EntryIndex = 0;
            }
            void OpenFile(const StringViewType filePath)
            {
                File = std::make_unique<fmt::ostream>(fmt::output_file(filePath.data()));
            }
            void ReserveStrings()
            {
                CollectionString.reserve(MinimumCollectionStringSize);
                TempString.reserve(MinimumTempStringSize);

                Messages.reserve(MinimumMessagesSize);
                Variables.reserve(MinimumVariablesSize);
                Tags.reserve(MinimumTagsSize);
                Source.reserve(MinimumSourceSize);
            }

        protected:
            static constexpr u32 MinimumCollectionStringSize = 512;
            static constexpr u32 MinimumTempStringSize = 128;
            static constexpr u32 MinimumMessagesSize = 256;
            static constexpr u32 MinimumVariablesSize = 256;
            static constexpr u32 MinimumTagsSize = 128;
            static constexpr u32 MinimumSourceSize = 128;

            //Deprecated
            //static constexpr u32 MinimumPartStringSize = 128;
    };

    namespace StaticLogTesting
    {
        class DefaultLogWriterPackageTester : public DefaultLogWriterPackage
        {
            public:
                using DefaultLogWriterPackage::Interpret;
                using DefaultLogWriterPackage::Formater;
                using DefaultLogWriterPackage::File;
                using DefaultLogWriterPackage::CollectionString;
                using DefaultLogWriterPackage::TempString;
                using DefaultLogWriterPackage::Messages;
                using DefaultLogWriterPackage::Variables;
                using DefaultLogWriterPackage::Tags;
                using DefaultLogWriterPackage::Source;
                using DefaultLogWriterPackage::EntryIndex;

                using DefaultLogWriterPackage::SetUp;
                using DefaultLogWriterPackage::OpenFile;
                using DefaultLogWriterPackage::ReserveStrings;

                using DefaultLogWriterPackage::MinimumCollectionStringSize;
                using DefaultLogWriterPackage::MinimumTempStringSize;
                using DefaultLogWriterPackage::MinimumMessagesSize;
                using DefaultLogWriterPackage::MinimumVariablesSize;
                using DefaultLogWriterPackage::MinimumTagsSize;
                using DefaultLogWriterPackage::MinimumSourceSize;


            public:
                DefaultLogWriterPackageTester(const StringType filePath) : DefaultLogWriterPackage(filePath) {}
                DefaultLogWriterPackageTester(DefaultLogWriterPackageTester PASS_RVALUE_REF) = default;
                DefaultLogWriterPackageTester REF operator=(DefaultLogWriterPackageTester PASS_RVALUE_REF) = default;

                DefaultLogWriterPackageTester(const DefaultLogWriterPackageTester PASS_REF) = delete;
                void operator=(const DefaultLogWriterPackageTester PASS_REF) = delete ;

        };

        DefaultLogWriterPackageTester REF GetDefPackageTester(DefaultLogWriterPackage PASS_REF package)
        {
            return static_cast<DefaultLogWriterPackageTester REF>(package);
        }
    }


    class DefaultLogWriter : public Logger<DefaultLogWriter, DefaultLogWriterPackage>
    {
        public:
            using ThisPackageType = DefaultLogWriterPackage;
            using ThisLoggerType = Logger<DefaultLogWriter, DefaultLogWriterPackage>;
            using ThisType = DefaultLogWriter;
            using StringType = ::std::string;
            using StringViewType = ::std::string_view;

        protected:
            static constexpr u32 LogIterationFieldSize = 8;
            static constexpr u32 LogClockFieldSize = 8;
            //TODO - Add list separation
            // - Add newline constexpr
            static constexpr char Separator = ' ';
            static constexpr bool DoAppOutputLog = false;


            //The obligatory function overloads
        public:
            template<typename ... MsgTypes>
            void WriteMsgs(const u32 level, const MsgTypes ... msgs)
            {
                this->AppendMsgs(msgs...);

                this->AddAll(level);

                this->PushLogOut();
            }
            template<typename ... NameTypes, typename ... VarTypes>
            void WriteVars(const u32 level, const NameTypes ... varNames, const VarTypes ... varValues)
            {
                this->AppendVars(varNames..., varValues...);

                this->AddAll(level);

                this->PushLogOut();
            }

            void AppendSource(const StringViewType file, const u32 lineNum)
            {
                AddFormatedSource(file, lineNum, this->Formater, this->Source);
                AddSeparator(this->Source);
            }
            template<typename ... TagTypes>
            void AppendTags(TagTypes ... tags)
            {
                this->UnravelAndAppendTags(tags...);
            }

            void OpenStream(const StringViewType path)
            {
                this->OpenFile(path);
            }            
            void CloseStream() = delete;

            bool IsStreamOpen()
            {
                return true;
            }
            void FlushStream() = delete;

            void OnConstruction()
            {
                this->LogConstructionMessage(ThisType::MaxLevelIndex);
            }
            void OnDestruction()
            {
                this->LogDestructionMessage(ThisType::MaxLevelIndex);
            }


            //Custom added public functions
        public:
            template<typename ... MsgTypes>
            void AppendMsgs(MsgTypes... msgs)
            {
                this->UnravelAndAddMsgs(msgs...);
            }

            template<typename ... StringArgumentTypes, typename ... ValueArgumentTypes>
            void AppendVars(const StringArgumentTypes ... stringArgs, const ValueArgumentTypes ... valueArgs)
            {
                this->UnravelAndAddVariables(stringArgs..., valueArgs...);
            }


        protected:
            void AddSource()
            {
                this->CollectionString.append(this->Source);
                AddSeparator(this->CollectionString);
            }
            void AddIterations()
            {
                AddFormatedIterationCount(this->EntryIndex, this->Formater, this->CollectionString);
                AddSeparator(this->CollectionString);
            }
            void AddTime()
            {
                time_t rawTime;
                time(&rawTime);

                AddFormatedTime(this->Formater, rawTime, clock(), this->CollectionString);
                AddSeparator(this->CollectionString);
            }
            void AddLvl(const u32 level)
            {
                AddFormatedLevel(level, this->Formater, this->CollectionString);
                AddSeparator(this->CollectionString);
            }
            void AddMsgs()
            {
                this->AddFormatedMsg(this->Messages, this->CollectionString);
                AddSeparator(this->CollectionString);
            }
            void AddVars()
            {
                this->CollectionString.append(this->Variables);
            }
            void AddTags()
            {
                this->CollectionString.append(this->Tags);
            }


        protected:
            void AddAll(const u32 level)
            {
                this->AddSource();
                this->AddIterations();
                this->AddTime();
                this->AddLvl(level);
                this->AddMsgs();
                this->AddVars();
                this->AddTags();
            }


        public:
            void LogConstructionMessage(const u32 level)
            {
                this->AppendMsgs("Log Starting................");
                this->AppendTags("log_construction", "log_starting");

                this->AddAll(level);

                this->PushLogOut();
            }
            void LogDestructionMessage(const u32 level)
            {
                this->AppendMsgs("Log Ending..................");
                this->AppendTags("log_destruction", "log_ending");

                this->AddAll(level);

                this->PushLogOut();
            }           
            void LogLoggerInfo(const u32 level)
            {
                this->AppendMsgs("Logger info : Default log writer; Displaying in order: 1-Code source 2-iteration count 3-Date and time 4-Level 5-Messages 6-variables 7-tags");
                this->AppendTags("log_info");

                this->AddAll(level);

                this->PushLogOut();
            }


        protected:
            inline void IncrementEntryIndex() {this->EntryIndex ++;}
            inline void PushLogOut()
            {
                this->AddNewline(this->CollectionString);

                this->File->print(this->CollectionString);

                if(DoAppOutputLog)
                    fmt::print(this->CollectionString);

                this->AfterLog();
            }
            inline void AfterLog()
            {
                this->IncrementEntryIndex();
                this->CleanUp();
            }
            inline void CleanUp()
            {
                this->ResetString(this->CollectionString);
                this->ResetString(this->Messages);
                this->ResetString(this->Variables);
                this->ResetString(this->Source);
                this->ResetString(this->Tags);
            }


        protected:
            template<typename FirstMsgType, typename ... MsgTypes>
            inline void UnravelAndAddMsgs(FirstMsgType firstMsg, MsgTypes ... msgs)
            {
                this->Interpret.InterpretArg(firstMsg, this->TempString);
                this->AddUnformatedMsgPart(this->TempString, this->Messages);
                this->UnravelAndAddMsgs(msgs...);
            }
            inline void UnravelAndAddMsgs()
            {

            }

            template<typename FirstStringArgument, typename FirstValueArgument,
                     typename ... StringArgumentTypes, typename ... ValueArgumentTypes>
            inline void UnravelAndAddVariables(const FirstStringArgument firstString, const FirstValueArgument firstValue,
                                                const StringArgumentTypes ... stringArgs, const ValueArgumentTypes ... valueArgs)
            {
                this->Interpret.InterpretArg(firstValue, this->TempString);
                this->AddFormatedVariable(firstString, firstValue, this->Variables);
                this->AddSeparator(this->Variables);
                this->UnravelAndAddVariables(stringArgs..., valueArgs...);
            }
            inline void UnravelAndAddVariables() {}


            template<typename FirstTagType, typename ... TagTypes>
            inline void UnravelAndAppendTags(FirstTagType firstTag, TagTypes ... tags)
            {
                this->AddFormatedTag(firstTag, this->Tags);
                this->AddSeparator(this->Tags);
                this->UnravelAndAppendTags(tags...);
            }
            inline void UnravelAndAppendTags()
            {

            }


        protected:
            inline void AddFormatedTime(fmt::format_int PASS_REF formater, const time_t rawTime, const clock_t clock, std::string PASS_REF output) const
            {
                char str[26];
                ctime_s(str, sizeof str, &rawTime);

                output += '[';
                output.append(str);
                output.pop_back(); //Removes th newline character
                output += " - ";
                this->PutNumIntoCharcterField(clock, this->LogIterationFieldSize, formater, output);
                output += ']';
            }

            inline void AddFormatedSource(const std::string_view file, const u32 lineNum, fmt::format_int PASS_REF formater, std::string PASS_REF output) const
            {
                output += file;
                output += " [";
                formater.format_unsigned(lineNum);
                output.append(formater.data(), formater.size());
                output += "]";
            }

            inline void AddFormatedIterationCount(const u32 iterations, fmt::format_int PASS_REF formater, std::string PASS_REF output) const
            {
                this->PutNumIntoCharcterField(iterations, ThisType::LogIterationFieldSize, formater, output);
            }

            inline void AddFormatedLevel(const u32 level, fmt::format_int PASS_REF formater, std::string PASS_REF output) const
            {
                output += '<';
                formater.format_unsigned(level);
                output.append(formater.data(), formater.size());
                output += '>';
            }

            inline void AddFormatedMsg(std::string_view msg, std::string PASS_REF output) const
            {
                output += "msg = ";
                output += msg;
            }

            inline void AddUnformatedMsgPart(std::string_view msg, std::string PASS_REF output) const
            {
                output += msg;
            }

            inline void AddFormatedTag(std::string_view tag, std::string PASS_REF output) const
            {
                output += "<";
                output += tag;
                output += ">";
            }

            inline void AddFormatedVariable(const std::string_view varName, const std::string_view varValue, std::string PASS_REF output) const
            {
                output += varName;
                output += " = ";
                output += varValue;
            }

            inline void AddSeparator(std::string PASS_REF output) const
            {
                output += Separator;
            }

            inline void AddNewline(std::string PASS_REF output) const
            {
                output += '\n';
            }


        protected:
            inline void PutNumIntoCharcterField(const unsigned long num, const u32 fieldLenght, fmt::format_int PASS_REF formater, std::string PASS_REF output) const
            {
                formater.format_unsigned(num);
                auto formatedSize = formater.size();

                //Only puts in the field if its size is bigger than the num as string size
                //Doesnt use if statements cause they are generally slow
                //Converts the the result of the comparison to i32 (false = 0; true = 1) and multiplies the size calculataion by it
                //This results in if the expression is untrue the result of the whole is always 0
                output.append(static_cast<u32>(fieldLenght > formatedSize) * (fieldLenght - formatedSize), '0');
                output.append(formater.data(), formatedSize);
            }
            inline void ResetString(std::string PASS_REF temp) const noexcept {temp.clear();}
    };

    namespace StaticLogTesting
    {
        struct DefaultLogWriterTester : public DefaultLogWriter
        {
            public:
                using DefaultLogWriter::Interpret;
                using DefaultLogWriter::Formater;
                using DefaultLogWriter::File;
                using DefaultLogWriter::CollectionString;
                using DefaultLogWriter::TempString;
                using DefaultLogWriter::Messages;
                using DefaultLogWriter::Variables;
                using DefaultLogWriter::Tags;
                using DefaultLogWriter::Source;
                using DefaultLogWriter::EntryIndex;

                using DefaultLogWriter::LogIterationFieldSize;
                using DefaultLogWriter::LogClockFieldSize;
                using DefaultLogWriter::Separator;
                using DefaultLogWriter::DoAppOutputLog;

                using DefaultLogWriter::OnConstruction;
                using DefaultLogWriter::OnDestruction;

                using DefaultLogWriter::AddSource;
                using DefaultLogWriter::AddIterations;
                using DefaultLogWriter::AddTime;
                using DefaultLogWriter::AddLvl;
                using DefaultLogWriter::AddMsgs;
                using DefaultLogWriter::AddVars;
                using DefaultLogWriter::AddTags;
                using DefaultLogWriter::AddAll;

                using DefaultLogWriter::IncrementEntryIndex;
                using DefaultLogWriter::PushLogOut;
                using DefaultLogWriter::AfterLog;
                using DefaultLogWriter::CleanUp;

                using DefaultLogWriter::UnravelAndAddMsgs;
                using DefaultLogWriter::UnravelAndAddVariables;
                using DefaultLogWriter::UnravelAndAppendTags;

                using DefaultLogWriter::AddFormatedTime;
                using DefaultLogWriter::AddFormatedSource;
                using DefaultLogWriter::AddFormatedIterationCount;
                using DefaultLogWriter::AddFormatedLevel;
                using DefaultLogWriter::AddFormatedMsg;
                using DefaultLogWriter::AddUnformatedMsgPart;
                using DefaultLogWriter::AddFormatedTag;
                using DefaultLogWriter::AddFormatedVariable;
                using DefaultLogWriter::AddSeparator;
                using DefaultLogWriter::AddNewline;
                using DefaultLogWriter::PutNumIntoCharcterField;
                using DefaultLogWriter::ResetString;

                /*
                DataInterpretType REF GetInterpret() {return this->Interpret;}
                fmt::format_int REF GetFormater() {return this->Formater;}
                std::unique_ptr<fmt::ostream> REF GetFile() {return this->File;}
                std::string REF GetCollectionString() {return this->CollectionString;}
                std::string REF GetTempString() {return this->TempString;}
                std::string REF GetMessages() {return this->Messages;}
                std::string REF GetVariables() {return this->Variables;}
                std::string REF GetTags() {return this->Tags;}
                std::string REF GetSource() {return this->Source;}
                u32 REF GetEntryIndex() {return this->EntryIndex;}
                */
        };

        DefaultLogWriterTester REF GetDefWriterTester(Logger<DefaultLogWriter> PASS_REF logger)
        {
            return static_cast<DefaultLogWriterTester REF>(logger.GetWriter());
        }
    }
}

#endif // STATICLOG_H
