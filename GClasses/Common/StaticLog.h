#ifndef STATICLOG_H
#define STATICLOG_H

#include "G_Common.h"

/*
template <typename Derived, typename DerivedDataPackage>
class CrtpBase : public DerivedDataPackage
{
        //The following functions are obligatory to be overloaded
    public:
        template <typename Type>
        inline void OverloadedFunc(Type arg)
        {
            static_cast<Derived POINTER>(this)->InetrpretArg(arg);
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

#include <array>
namespace StaticLogger
{

    using StringType = ::std::string;
    using StringViewType = ::std::string_view;

    template <typename T1, typename T2,//DerivedDataPackage must be a derived class from Logger
              std::enable_if_t<std::is_same_v<T1, T2>, int> = 0>
    struct RequireSameType {};

    template <typename Derived,
              typename DerivedDataPackage = typename Derived::ThisPackageType>
    class Logger : public DerivedDataPackage, public LogWriterTraits
    {
            //static constexpr RequireSameType<DerivedDataPackage, typename Derived::ThisPackageType> Require; //No type named ThisPackageType in Derived.... - has to be something with crtp

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
                LevelLoggingStatus.fill(true);
                GetWriter().OnConstruction();
            }
            ~Logger()
            {
                GetWriter().OnDestruction();
            }


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
    };



    namespace StaticLogTesting
    {

        struct TestingDataPackage1
        {
            StringType FileName;
            u32 LineNum;
            StringType Tags;
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

                void OnConstruction()                                       {this->LastFunctionCalled = "OnConstruction";}
                void OnDestruction()                                        {this->LastFunctionCalled = "OnDestruction";}

        };

    }




    /*
    //The default logger interpret
    struct DefaultLogWiriterPackage
    {
        public:
            std::ofstream File;
            std::string CollectionString;
            std::string TempString;
            u32 EntryIndex;

            //Parts
            std::string SourcePartString;
            std::string LevelPartString;
            std::string MessagesPartString;
            std::string VariablesPartString;


        public:
            static constexpr u32 MinimumCollectionStringSize = 512;
            static constexpr u32 MinimumTempStringSize = 256;
            static constexpr u32 MinimumPartStringSize = 128;
            static constexpr u32 LogIterationFieldSize = 8;
            static constexpr u32 LogClockFieldSize = 8;
            static constexpr char Separator = ' ';
    };

    class DefaultLogWiriter : public Logger<DefaultLogWiriter, DefaultLogWiriterPackage>
    {
        public:
            using ThisPackageType = StaticLogger::DefaultLogWiriterPackage;

            //The obligatory function overloads
        public:
    };
    */


}

#endif // STATICLOG_H
